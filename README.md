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