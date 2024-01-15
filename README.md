# Multi-Threaded-Queue

Implement the data structure queue where data is enqueued or dequeued in multiple threads. The queue is shared by two roles of threads - consumer and producer. The former keeps to insert integer values while the latter consumes the inserted integers.

Constraints:
1. Producer thread produces an element and add it to the queue, it does not release the queue until the queue becomes full.

2. Consumer thread consumes an element from the queue, it does not release the queue until it consumes the entire queue.

3. Consumer signals the producers when queue is exhausted, producer signals the consumers when queue becomes full.
