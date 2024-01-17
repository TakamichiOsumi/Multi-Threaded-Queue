#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "Queue.h"

static void
app_test_basic_queue_operations(void){
    Queue *q = queue_init();

    if (queue_is_empty(q) != true){
	fprintf(stderr,
		"the queue is not empty after its generation\n");
	exit(-1);
    }

    queue_enqueue(q, (void *) 1);
    queue_check_len(q, 1);
    queue_enqueue(q, (void *) 2);
    queue_enqueue(q, (void *) 3);
    queue_check_len(q, 3);
    queue_enqueue(q, (void *) 4);
    queue_enqueue(q, (void *) 5);

    if (queue_is_full(q) != true){
	fprintf(stderr, "the queue is not full.\n");
	exit(-1);
    }

    printf("Deque : %lu\n", (uintptr_t) queue_dequeue(q));
    printf("Deque : %lu\n", (uintptr_t) queue_dequeue(q));

    queue_check_len(q, 3);

     printf("Deque : %lu\n", (uintptr_t) queue_dequeue(q));
     printf("Deque : %lu\n", (uintptr_t) queue_dequeue(q));
     printf("Deque : %lu\n", (uintptr_t) queue_dequeue(q));

     if (queue_is_empty(q) != true){
	 fprintf(stderr, "the queue is not empty after full deque\n");
	 exit(-1);
     }

     if (queue_dequeue(q) != NULL){
	 fprintf(stderr, "the queue is empty but deque returned a non-null value\n");
	 exit(-1);
     }

     queue_free(q);
}

static void
app_compare_queue_and_array(Queue *q, int **ary){
    int i = 0;
    bool failed = false;

    if (!q || !ary){
	fprintf(stderr, "input data is missing.\n");
	exit(-1);
    }

    for (; i < Q_DEFAULT_SIZE; i++){
	if (q->elem[i] != (void *) ary[i]){
	    fprintf(stderr,
		    "CMP NG : %lu and %lu.\n",
		    (uintptr_t) q->elem[i], (uintptr_t) ary[i]);
	    failed = true;
	}
    }

    if (failed){
	fprintf(stderr,
		"Comparison between queue and array has been failed.\n");
	exit(-1);
    }
}

static void
app_test_bulk_opearations(void){
    Queue *q = queue_init();
    int *enqueue_ary[Q_DEFAULT_SIZE] = {(void *) 100, (void *) 101,
					(void *) 102, (void *) 103, (void *) 104};
    int **dequeue_ary, i;
    bool failed = false;
    char *status;
    uintptr_t enqueued_val, dequeued_val;

    queue_bulk_enqueue(q, (void **) enqueue_ary);
    queue_print(q);
    dequeue_ary = (int **) queue_bulk_dequeue(q);
    queue_check_len(q, 0);

    for (i = 0; i < Q_DEFAULT_SIZE; i++){
	enqueued_val = (uintptr_t) enqueue_ary[i];
	dequeued_val = (uintptr_t) dequeue_ary[i];
	if (enqueued_val == dequeued_val)
	    status = "OK";
	else{
	    status = "NG";
	    failed = true;
	}
	printf("[%s] CMP %s : enqueue_ary[%d] = %lu & dequeue_ary[%d] = %lu\n",
	       __FUNCTION__, status, i, enqueued_val, i, dequeued_val);
    }

    free(dequeue_ary);
    queue_free(q);

    if (failed){
	fprintf(stderr, "testing bulk enqueue or bulk dequeue has failed\n");
	exit(-1);
    }
}

static void
app_test_ring_buffer_functionality(void){
    Queue *q = queue_init();
    /* Datasets for enqueue */
    int *enqueue_ary[Q_DEFAULT_SIZE] = { (void *) 100, (void *) 101,
					 (void *) 102, (void *) 103, (void *) 104};
    int *enqueue_ary2[Q_DEFAULT_SIZE] = { (void *) 5, (void *) 6,
					  (void *) 7, (void *) 8, (void *) 9};
    /* Expected results for ring buffer feature */
    int *expected_ary[Q_DEFAULT_SIZE] = { (void *) 1,(void *) 101,
					  (void *) 102, (void *) 103,(void *) 104};
    int *expected_ary2[Q_DEFAULT_SIZE] = { (void *) 1, (void *) 2, NULL, NULL, NULL};
    int *expected_ary3[Q_DEFAULT_SIZE] = { NULL, NULL, NULL, NULL, NULL};
    int *expected_ary4[Q_DEFAULT_SIZE] = { (void *) 8, (void *) 9,
					   (void *) 5, (void *) 6, (void *) 7};

    queue_bulk_enqueue(q, (void **) enqueue_ary);
    queue_print(q);
    queue_dequeue(q);
    queue_enqueue(q, (void *) 1);
    queue_print(q);
    app_compare_queue_and_array(q, expected_ary);
    queue_dequeue(q);
    queue_dequeue(q);
    queue_dequeue(q);
    queue_dequeue(q);
    /* Show the first bulk inserted data has been removed */
    queue_print(q);

    queue_enqueue(q, (void *) 2);
    /* Here, q->elem[0] == 1 and q->elem[1] == 2 */
    queue_print(q);
    app_compare_queue_and_array(q, expected_ary2);
    queue_dequeue(q);
    queue_dequeue(q);
    app_compare_queue_and_array(q, expected_ary3);
    /*
     * All data in the queue has been dequeued,
     * but the internal pointer to enqueue a data next
     * is in the middle of the queue. Thus, the next
     * bulk enqueue will result in the shifted result.
     */
    queue_bulk_enqueue(q, (void **) enqueue_ary2);
    app_compare_queue_and_array(q, expected_ary4);
    queue_print(q);

    queue_free(q);
}

int
main(int argc, char **argv){

    printf("<Execute basic operation test>\n");
    app_test_basic_queue_operations();

    printf("<Execute bulk operation test>\n");
    app_test_bulk_opearations();

    printf("<Execute ring buffer test>\n");
    app_test_ring_buffer_functionality();

    return 0;
}
