# Basic pthreads examples

This repository contains a few examples of using pthreads in C.

## Problems

### [1. Hello Pthreads](./hello_pthreads/)

- In `threads.c`, we create multiple threads that are updating a global variable.
- In `threads_with_mutex.c`, we use a mutex to ensure that only one thread is updating the global variable at a time.

### [2. Argumentive Pthreads](./argumentive_pthreads/)

- In `nlocks.c`, we are using pthreads along with arguments and it does the following things:
  - Creates and initializes 10 shared counters and 10 locks.
  - The parent process creates 10 threads.
  - Each thread, adds 1 to its corresponding data value, a 1000 times. E.g., thread 0 updates data[0].
  - The parent also updates the data items in the following manner—adds 1 to data[0], then to data[1] etc. for all 10 data items and then repeats for a total of 1000 iterations.
  - With correct synchronization each of the data values should be 2000.

### [3. Task Server with a Thread Pool](./taskqueue/)

- In `taskqueue.c`, we are using multiple threads to process a queue of tasks.
- It has following features:
  - The main thread reads the number of worker threads as a command line argument and creates a pool of worker threads.
  - The main thread reads the tasks one by one from the file and enters the task into a queue.
  - A free thread from the thread pool picks up the task and processes it.
  - Once all tasks are complete, the main thread joins the other threads and the global variables are printed.
