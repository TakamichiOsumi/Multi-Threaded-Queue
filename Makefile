CC	= gcc
CFLAGS	= -O0 -Wall
TEST	= run_queue_test
PROGRAM	= run_multi-threaded-queue

all: $(TEST) $(PROGRAM)

$(TEST)	: Queue.o
	$(CC) $(CFLAGS) app_test_queue.c $< -o $(TEST)

$(PROGRAM) : Queue.o
	$(CC) $(CFLAGS) threaded_prod_and_cons.c $< -o $(PROGRAM)

Queue.o	:
	$(CC) $(CFLAGS) -c Queue.c

.PHONY:	test clean

test: $(TEST)
	@echo "Testing queue is successful when it returns 0."
	@./$(TEST) &> /dev/null && echo ">>> $$?"

clean	:
	@rm -f *.o $(TEST) $(PROGRAM)
