#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "Queue.h"

static void *
queue_malloc(size_t size){
    void *p;

    if ((p = malloc(size)) == NULL){
	perror("malloc");
	exit(-1);
    }

    return p;
}

Queue *
queue_init(void){
    Queue *q = queue_malloc(sizeof(Queue));

    q->front = q->rear = q->count = 0;
    pthread_mutex_init(&q->q_mutex, NULL);
    pthread_cond_init(&q->q_cv, NULL);

    return q;
}

bool
queue_is_empty(Queue *q){
    if (q == NULL)
	return false;

    if (Q_COUNT(q) == 0)
	return true;
    else
	return false;
}

bool
queue_is_full(Queue *q){
    if (q == NULL)
	return false;

    if (Q_COUNT(q) == Q_DEFAULT_SIZE)
	return true;
    else
	return false;
}

/* Support the ring buffer functionality in enqueue function */
bool
queue_enqueue(Queue *q, void *ptr){
    if (q == NULL || ptr == NULL
	|| queue_is_full(q)){
	return false;
    }else{
	q->elem[q->rear] = ptr;
	q->rear = (q->rear + 1) % Q_DEFAULT_SIZE;
	q->count++;
	return true;
    }
}

/* Support the ring buffer functionality in dequeue function */
void *
queue_dequeue(Queue *q){
    if (q == NULL || queue_is_empty(q)){
	return NULL;
    }else{
	void *element;

	element = q->elem[q->front];
	q->elem[q->front] = NULL;
	q->front = (q->front + 1) % Q_DEFAULT_SIZE;
	q->count--;

	return element;
    }
}

void
queue_print(Queue *q){
    int i = 0;

    printf("----- <QUEUE> -----\n");
    if (q == NULL) return;
    for (; i < Q_DEFAULT_SIZE; i++){
	printf("q->elem[%d] : %lu\n", i, (uintptr_t) q->elem[i]);
    }
}

void
queue_check_len(Queue *q, int expected){
    if (!q){
	fprintf(stderr, "the queue is null. do nothing.\n");
	return;
    }

    if (Q_COUNT(q) != expected){
	fprintf(stderr,
		"expected the queue count to be %d, but it was %d.\n",
		expected, Q_COUNT(q));
    }
}

void
queue_bulk_enqueue(Queue *q, void **data){
    int i;

    printf("debug : before pthread_mutex_lock() in %s\n", __FUNCTION__);
    pthread_mutex_lock(&q->q_mutex);

    /*
     * See the explanation of this 'while' in queue_bulk_dequeue().
     *
     * When queue_bulk_dequeue() calls pthread_cond_broadcast(),
     * it's possible that other thread fills up the queue before
     * this thread starts to execute. Then, run another iteration
     * of queue_is_full() and ensure that we have the queue empty
     * in this thread for the critial section below.
     */
    while(queue_is_full(q))
	pthread_cond_wait(&q->q_cv, &q->q_mutex);
    assert(queue_is_empty(q));

    /* Critial section */
    for(i = 0; i < Q_DEFAULT_SIZE; i++){
	queue_enqueue(q, data[i]);
	printf("debug : %s : data[%d] : %p, q->elem[%d] : %p\n",
	       __FUNCTION__, i, data[i], i, q->elem[i]);
    }
    pthread_cond_broadcast(&q->q_cv);
    pthread_mutex_unlock(&q->q_mutex);
    printf("debug : after pthread_mutex_unlock() in %s\n", __FUNCTION__);
}

/*
 * Caller's responsibility to free the returned array.
 */
void **
queue_bulk_dequeue(Queue *q){
    int i;
    void **p;

    p = queue_malloc(sizeof(void *) * Q_DEFAULT_SIZE);

    printf("\tdebug : before pthread_mutex_lock() in %s\n", __FUNCTION__);
    pthread_mutex_lock(&q->q_mutex);

    /*
     * After pthread_mutex_unlock() in queue_bulk_enqueue(),
     * it's possible that other waiting thread gets unlocked first
     * before this thread, and then uses up the entire queue.
     *
     * It means this bulk dequeue trys to drain the empty queue,
     * but it's not desirable situation.
     *
     * 'while' statement that wraps pthread_cond_wait() below
     * leads to another predicate check iteration when this
     * thread starts to execute and prevents the undesirable
     * situation from happening. (Also, assertion failure
     * below happens if we utilize 'if' statement here.)
     */
    while(queue_is_empty(q))
	pthread_cond_wait(&q->q_cv, &q->q_mutex);
    assert(queue_is_full(q));

    /* Critial section */
    for (i = 0; i < Q_DEFAULT_SIZE; i++){
	p[i] = queue_dequeue(q);
	printf("\tdebug : %s : p[%d] : %p\n", __FUNCTION__, i, p[i]);
    }
    pthread_cond_broadcast(&q->q_cv);
    pthread_mutex_unlock(&q->q_mutex);
    printf("\tdebug : after pthread_mutex_unlock() in %s\n", __FUNCTION__);

    return p;
}

void
queue_free(Queue *q){
    if (q != NULL)
	free(q);
}
