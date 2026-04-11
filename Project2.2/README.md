`reader_count` is modified by multiple reader threads simultaneously. Since the whole point of a reader lock is that many readers can be in the critical section at once, there's no mutual exclusion protecting it, so the increment/decrement must be atomic to avoid races.

`writer_count`, on the other hand, is only ever touched inside the writer critical section, which is mutually exclusive by definition. Only one writer can be in there at a time, so no two threads ever touch `writer_count` concurrently, a plain `int` is fine.

------------------------------

The `write_lock` only succeeds when `lock_state` == 0 (no readers, no writer). A writer waiting to acquire the lock needs a window where the reader count drains to zero.
If there's a steady stream of readers arriving, the sequence can look like this:

```
Writer waiting...
Reader A enters  → lock_state = 1
Reader B enters  → lock_state = 2
Reader A leaves  → lock_state = 1
Reader C enters  → lock_state = 2
Reader B leaves  → lock_state = 1
...
```

The reader count never hits zero, so the writer's CAS never succeeds and it spins forever. New readers don't know or care that a writer is waiting, they just see the write bit is clear and pile in. The writer has no way to signal "stop letting readers in" without an additional mechanism like a writer-pending bit.