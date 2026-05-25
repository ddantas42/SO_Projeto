const CAPACITY = 10;
const BOARD_COLS = 5;

const ITEM_NAMES = ["ROCK", "DIRT", "SAND", "OBSIDIAN", "WATER"];
const ITEM_COLORS = ["#9ca3af", "#d97706", "#f59e0b", "#7c3aed", "#06b6d4"];

const boardGrid = document.getElementById("board-grid");
const boardMeta = document.getElementById("board-meta");
const statusEl = document.getElementById("status");
const eventTitle = document.getElementById("event-title");
const eventTime = document.getElementById("event-time");
const eventNote = document.getElementById("event-note");
const logList = document.getElementById("log-list");

const stepSlider = document.getElementById("step-slider");
const speedSlider = document.getElementById("speed-slider");
const speedLabel = document.getElementById("speed-label");

const loadDefaultBtn = document.getElementById("load-default");
const csvFileInput = document.getElementById("csv-file");

const prevBtn = document.getElementById("prev-btn");
const nextBtn = document.getElementById("next-btn");
const playBtn = document.getElementById("play-btn");
const resetBtn = document.getElementById("reset-btn");

const slots = [];
let snapshots = [];
let baseTimeNs = 0;
let currentStep = 0;
let isPlaying = false;
let playTimer = null;
const MIN_FRAME_DELAY_MS = 16;
const LOG_MAX_LINES = 250;

function ns(sec, nsec) {
  return Number(sec) * 1_000_000_000 + Number(nsec);
}

function parseCsv(text) {
  const lines = text.trim().split(/\r?\n/);
  if (lines.length < 2) {
    throw new Error("CSV must have header and at least one row");
  }

  const headers = lines[0].split(",").map((h) => h.trim());
  const index = Object.fromEntries(headers.map((h, i) => [h, i]));
  const required = [
    "analysis_thread_id",
    "collector_id",
    "item_type",
    "deposited_sec",
    "deposited_nsec",
    "analysis_begin_sec",
    "analysis_begin_nsec",
    "analysis_end_sec",
    "analysis_end_nsec",
  ];

  for (const key of required) {
    if (!(key in index)) {
      throw new Error(`Missing CSV column: ${key}`);
    }
  }

  return lines.slice(1).filter(Boolean).map((line, rowIndex) => {
    const cols = line.split(",").map((c) => c.trim());
    return {
      rowIndex,
      analysisThreadId: Number(cols[index.analysis_thread_id]),
      collectorId: Number(cols[index.collector_id]),
      itemType: Number(cols[index.item_type]),
      depositedNs: ns(cols[index.deposited_sec], cols[index.deposited_nsec]),
      analysisBeginNs: ns(cols[index.analysis_begin_sec], cols[index.analysis_begin_nsec]),
      analysisEndNs: ns(cols[index.analysis_end_sec], cols[index.analysis_end_nsec]),
    };
  });
}

function buildEvents(records) {
  const events = [];
  for (const record of records) {
    events.push({ timeNs: record.depositedNs, priority: 0, type: "deposit", record });
    events.push({ timeNs: record.analysisBeginNs, priority: 1, type: "analysis_start", record });
    events.push({ timeNs: record.analysisEndNs, priority: 2, type: "analysis_end", record });
  }

  events.sort((a, b) => {
    if (a.timeNs !== b.timeNs) return a.timeNs - b.timeNs;
    if (a.priority !== b.priority) return a.priority - b.priority;
    return a.record.rowIndex - b.record.rowIndex;
  });

  return events;
}

function buildTimeline(records) {
  const events = buildEvents(records);
  const board = Array(CAPACITY).fill(null);
  const queue = [];
  const outRing = [];
  const snapshotsLocal = [];

  let inIndex = 0;
  let outIndex = 0;
  let count = 0;

  for (const event of events) {
    let note = "";

    if (event.type === "deposit") {
      const slot = inIndex;
      const placed = { slot, record: event.record };
      board[slot] = placed;
      queue.push(placed);
      outRing.push(slot);

      inIndex = (inIndex + 1) % CAPACITY;
      count++;
      note = `Deposited collector ${event.record.collectorId} (${ITEM_NAMES[event.record.itemType] ?? "UNKNOWN"}) in slot ${slot}`;
    } else if (event.type === "analysis_start") {
      if (queue.length > 0) {
        const first = queue.shift();
        board[first.slot] = null;
        outRing.shift();
        outIndex = (outIndex + 1) % CAPACITY;
        count--;
        note = `Analysis started for slot ${first.slot} (collector ${first.record.collectorId})`;
      } else {
        note = "Analysis started while board was empty";
      }
    } else {
      note = `Analysis finished by thread ${event.record.analysisThreadId} for collector ${event.record.collectorId}`;
    }

    snapshotsLocal.push({
      event,
      board: board.map((slot) => (slot ? { ...slot } : null)),
      inIndex,
      outIndex,
      count,
      note,
    });
  }

  return snapshotsLocal;
}

function formatDelta(nsValue) {
  const delta = Math.max(nsValue - baseTimeNs, 0);
  const totalMs = Math.floor(delta / 1_000_000);
  const minutes = Math.floor(totalMs / 60000);
  const seconds = Math.floor((totalMs % 60000) / 1000);
  const millis = totalMs % 1000;
  return `+${String(minutes).padStart(2, "0")}:${String(seconds).padStart(2, "0")}.${String(millis).padStart(3, "0")}`;
}

function createBoardGrid() {
  boardGrid.innerHTML = "";
  slots.length = 0;

  for (let i = 0; i < CAPACITY; i++) {
    const slot = document.createElement("div");
    slot.className = "slot";
    slot.innerHTML = `<div class="slot-id">slot ${i}</div><div class="slot-main">EMPTY</div>`;
    boardGrid.appendChild(slot);
    slots.push(slot);
  }
}

function clearLog() {
  logList.innerHTML = '<div class="log-empty">Load data and press Play to start logging events.</div>';
}

function appendLogLine(snapshot) {
  const line = document.createElement("div");
  line.className = "log-line fresh";
  const eventType = snapshot.event.type.replace("_", " ");
  line.textContent = `[${formatDelta(snapshot.event.timeNs)}] [${eventType}] ${snapshot.note}`;

  if (logList.querySelector(".log-empty")) {
    logList.innerHTML = "";
  }

  logList.appendChild(line);
  logList.scrollTop = logList.scrollHeight;

  setTimeout(() => {
    line.classList.remove("fresh");
  }, 1000);

  while (logList.children.length > LOG_MAX_LINES) {
    logList.removeChild(logList.firstElementChild);
  }
}

function renderStep(step, shouldLog = false) {
  if (!snapshots.length) return;

  currentStep = Math.max(0, Math.min(step, snapshots.length - 1));
  const snap = snapshots[currentStep];

  for (let i = 0; i < CAPACITY; i++) {
    const slot = slots[i];
    const state = snap.board[i];

    if (!state) {
      slot.classList.remove("filled");
      slot.style.backgroundColor = "";
      slot.querySelector(".slot-main").textContent = "EMPTY";
      continue;
    }

    const itemName = ITEM_NAMES[state.record.itemType] ?? "UNKNOWN";
    slot.classList.add("filled");
    slot.style.backgroundColor = ITEM_COLORS[state.record.itemType] ?? "#94a3b8";
    slot.querySelector(".slot-main").textContent = `C${state.record.collectorId} / A${state.record.analysisThreadId} / ${itemName}`;
  }

  boardMeta.textContent = `step=${currentStep + 1}/${snapshots.length} | count=${snap.count} | in=${snap.inIndex} | out=${snap.outIndex}`;
  eventTitle.textContent = snap.event.type.replace("_", " ");
  eventTime.textContent = `timestamp ${formatDelta(snap.event.timeNs)}`;
  eventNote.textContent = snap.note;

  stepSlider.value = String(currentStep);

  if (shouldLog) {
    appendLogLine(snap);
  }
}

function setControlsEnabled(enabled) {
  for (const el of [prevBtn, nextBtn, playBtn, resetBtn, stepSlider]) {
    el.disabled = !enabled;
  }
}

function stopPlayback() {
  isPlaying = false;
  if (playTimer) {
    clearTimeout(playTimer);
    playTimer = null;
  }
  playBtn.textContent = "Play";
}

function schedulePlayback() {
  if (!isPlaying || snapshots.length === 0) return;

  if (currentStep >= snapshots.length - 1) {
    stopPlayback();
    return;
  }

  const speed = Number(speedSlider.value) || 1;
  const currentEventNs = snapshots[currentStep].event.timeNs;
  const nextEventNs = snapshots[currentStep + 1].event.timeNs;
  const deltaMs = Math.max(0, (nextEventNs - currentEventNs) / 1_000_000);
  const delay = Math.max(MIN_FRAME_DELAY_MS, Math.round(deltaMs / speed));

  playTimer = setTimeout(() => {
    renderStep(currentStep + 1, true);
    schedulePlayback();
  }, delay);
}

function loadRecords(records) {
  snapshots = buildTimeline(records);
  if (!snapshots.length) {
    statusEl.textContent = "No events produced from CSV.";
    setControlsEnabled(false);
    return;
  }

  baseTimeNs = Math.min(...records.map((r) => r.depositedNs));
  stepSlider.max = String(snapshots.length - 1);
  statusEl.textContent = `Loaded ${records.length} rows, ${snapshots.length} timeline events.`;
  setControlsEnabled(true);
  stopPlayback();
  clearLog();
  renderStep(0, true);
}

async function loadFromDefaultPath() {
  statusEl.textContent = "Loading ../analysis_results.csv ...";
  try {
    const res = await fetch("../analysis_results.csv", { cache: "no-store" });
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    const text = await res.text();
    loadRecords(parseCsv(text));
  } catch (err) {
    statusEl.textContent = `Failed to load default CSV (${err.message}). Use Choose CSV.`;
  }
}

loadDefaultBtn.addEventListener("click", loadFromDefaultPath);

csvFileInput.addEventListener("change", async (event) => {
  const file = event.target.files?.[0];
  if (!file) return;

  statusEl.textContent = `Loading ${file.name} ...`;
  try {
    const text = await file.text();
    loadRecords(parseCsv(text));
  } catch (err) {
    statusEl.textContent = `Could not parse CSV (${err.message}).`;
    setControlsEnabled(false);
  }
});

prevBtn.addEventListener("click", () => {
  stopPlayback();
  renderStep(currentStep - 1, true);
});

nextBtn.addEventListener("click", () => {
  stopPlayback();
  renderStep(currentStep + 1, true);
});

resetBtn.addEventListener("click", () => {
  stopPlayback();
  clearLog();
  renderStep(0, true);
});

playBtn.addEventListener("click", () => {
  if (isPlaying) {
    stopPlayback();
    return;
  }

  if (snapshots.length && currentStep >= snapshots.length - 1) {
    renderStep(0);
  }

  isPlaying = true;
  playBtn.textContent = "Pause";
  schedulePlayback();
});

stepSlider.addEventListener("input", (event) => {
  stopPlayback();
  renderStep(Number(event.target.value), true);
});

speedSlider.addEventListener("input", () => {
  speedLabel.textContent = `${Number(speedSlider.value).toFixed(1)}x`;
});

createBoardGrid();
setControlsEnabled(false);
clearLog();
