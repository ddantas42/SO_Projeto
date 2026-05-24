# SO_Projeto

## MAIN PROCESS
  ├── creates shared memory
  ├── initializes board
  ├── forks Explorer process
  ├── forks Analysis process
  └── waits + cleanup

## EXPLORER PROCESS (Producer side)
  ├── generate sample (id, type, collected_time)
  ├── wait if count == STORAGE_CAPACITY
  ├── write to board->samples[in]
  ├── in = (in + 1) % STORAGE_CAPACITY
  ├── count++
  └── repeat

## ANALYSIS PROCESS (Consumer)
  ├── wait if count == 0
  ├── sample = board->samples[out]
  ├── begin_analising_time = now
  ├── simulate processing
  ├── end_analising_time = now
  ├── out = (out + 1) % STORAGE_CAPACITY
  ├── count--
  └── repeat

## SHARED BUFFER
produce → write[in] → in++ → count++
consume → read[out] → out++ → count--