# SO_Projeto

## Main Process
- Creates shared memory
- Initializes board
- Forks **Explorer** process (producer)
- Forks **Analysis** process (consumer)
- Waits for children and performs cleanup

---

## Explorer Process (Producer)
- Generate sample `(id, type, collected_time)`
- Wait if `count == STORAGE_CAPACITY`
- Write to `board->samples[in]`
- Update index:
  - `in = (in + 1) % STORAGE_CAPACITY`
- Increment `count`
- Repeat

---

## Analysis Process (Consumer)
- Wait if `count == 0`
- Read sample from `board->samples[out]`
- Set `begin_analysis_time = now`
- Simulate processing
- Set `end_analysis_time = now`
- Update index:
  - `out = (out + 1) % STORAGE_CAPACITY`
- Decrement `count`
- Repeat

---

## Shared Buffer Logic

```c
produce:
  write[in]
  in = (in + 1) % STORAGE_CAPACITY
  count++

consume:
  read[out]
  out = (out + 1) % STORAGE_CAPACITY
  count--

---

## CSV Board Visualizer

The run now exports `analysis_results.csv` with timestamps for deposit/start/end analysis.

Use the visual replay tool to reconstruct board state at each event:

```bash
python3 board_visualizer.py
```

Optional headless summary (useful in remote terminals):

```bash
python3 board_visualizer.py analysis_results.csv --headless
```

---

## Web Visualizer (HTML/CSS/JS)

Open the new frontend visualizer from the `visualizer/` folder.

Run a local static server from project root:

```bash
python3 -m http.server 8000
```

Then open:

`http://localhost:8000/visualizer/`

Inside the page you can:
- Load `../analysis_results.csv` directly
- Or choose any CSV file manually
- Replay each timestamped board event with slider/play controls