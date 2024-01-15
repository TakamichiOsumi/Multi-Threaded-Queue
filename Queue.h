#ifndef __QUEUE__
#define __QUEUE__

#include <stdbool.h>
#include <pthread.h>

typedef enum THREAD_TYPE {
    CONSUMER = 0,
    PRODUCER
} THREAD_TYPE;

#define Q_DEFAULT_SIZE 5
typedef struct Queue {
    /* Main data */
    void *elem[Q_DEFAULT_SIZE];
    /* Points to the first element */
    unsigned int front;
    /* Points to the last element */
    unsigned int rear;
    /* Number of data in the queue */
    unsigned int count;
    pthread_mutex_t q_mutex;
    pthread_cond_t q_cv;
} Queue;

#define Q_COUNT(q) (q->count)

Queue *queue_init(void);
bool queue_is_empty(Queue *q);
bool queue_is_full(Queue *q);
bool queue_enqueue(Queue *q, void *ptr);
void *queue_dequeue(Queue *q);
void queue_print(Queue *q);
void queue_check_len(Queue *q, int expected);
void queue_bulk_enqueue(Queue *q, void **data);
void **queue_bulk_dequeue(Queue *q);
void queue_free(Queue *q);

#endif
