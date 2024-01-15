#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Queue.h"

static pthread_t producers[2];
static pthread_t consumers[2];
static Queue *q;

static int
calc_sum(int **ary){
    int i, sum = 0;

    for(i = 0; i < Q_DEFAULT_SIZE; i++){
	sum = sum + (uintptr_t) ary[i];
    }
    return sum;
}

static void *
consumer_thread_main(void *arg){
    int **ary;
    uintptr_t thread_no = (uintptr_t) arg;

    printf("\tdebug : consumer (id=%lu) will start\n", thread_no);
    while(1){
	ary = (int **) queue_bulk_dequeue(q);
	printf("\tdebug : consumer (id=%lu)'s sum : %d\n",
	       thread_no, calc_sum(ary));
	/* Cleanup of the data in queue_bulk_dequeue() */
	free(ary);
    }
    return NULL;
}

static void *
producer_thread_main(void *arg){
    int i, *ary[Q_DEFAULT_SIZE] = { (void *) 1, (void *) 2,
				    (void *) 3, (void *) 4, (void *) 5};
    uintptr_t thread_no = (uintptr_t) arg;

    printf("debug : producer (id=%lu) will start\n", thread_no);

    /* Create an unique set of data per thread based on the argument */
    for (i = 0; i < Q_DEFAULT_SIZE; i++)
	ary[i] = (void *)((uintptr_t) ary[i] * thread_no);

    while(1){
	queue_bulk_enqueue(q, (void **) ary);
    }

    return NULL;
}

static pthread_t *
launch_queue_threads(pthread_t *qthread, THREAD_TYPE type, void *args){
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,
				PTHREAD_CREATE_DETACHED);
    switch(type){
	case CONSUMER:
	    if (pthread_create(qthread, &attr,
			       consumer_thread_main, args)){
		perror("pthread_create");
		exit(-1);
	    }
	    break;
	case PRODUCER:
	    if (pthread_create(qthread, &attr,
			       producer_thread_main, args)){
		perror("pthread_create");
		exit(-1);
	    }
	    break;
	default:
	    fprintf(stderr, "Detected unknown thread type.\n");
	    exit(-1);
	    break;
    }

    return qthread;
}

int
main(int argc, char **argv){

    q = queue_init();

    (void)launch_queue_threads(&producers[0], PRODUCER, (void *) 1);
    (void)launch_queue_threads(&producers[1], PRODUCER, (void *) 2);
    (void)launch_queue_threads(&consumers[1], CONSUMER, (void *) 3);
    (void)launch_queue_threads(&consumers[0], CONSUMER, (void *) 4);

    /* Let all threads work after creation */
    pthread_exit(0);

    return 0;
}
