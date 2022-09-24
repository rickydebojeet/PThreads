#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

struct read_write_lock
{
	// If required, use this strucure to create
	// reader-writer lock related variables
	pthread_mutex_t readLock;
	pthread_mutex_t writeLock;
	pthread_mutex_t readTry;
	pthread_mutex_t lock;
	int readCount;
	int writeCount;
	int waitingReaders;
	int waitingWriters;
} rwlock;

typedef struct
{
	int delay;
	int id;

} argument_t;

long int data = 0; //	Shared data variable

void InitalizeReadWriteLock(struct read_write_lock *rw)
{
	//	Initialize your reader-writer lock variables
	pthread_mutex_init(&rw->readLock, NULL);
	pthread_mutex_init(&rw->writeLock, NULL);
	pthread_mutex_init(&rw->lock, NULL);
	pthread_mutex_init(&rw->readTry, NULL);
	rw->readCount = 0;
	rw->writeCount = 0;
	rw->waitingReaders = 0;
	rw->waitingWriters = 0;
}

void DestroyReadWriteLock(struct read_write_lock *rw)
{
	//	Destroy your reader-writer lock variables
	pthread_mutex_destroy(&rw->readLock);
	pthread_mutex_destroy(&rw->writeLock);
	pthread_mutex_destroy(&rw->lock);
	pthread_mutex_destroy(&rw->readTry);
}

// The pthread based reader lock
void ReaderLock(struct read_write_lock *rw)
{
	//	Implement reader lock
	pthread_mutex_lock(&rw->readTry);
	pthread_mutex_lock(&rw->readLock);
	rw->readCount++;
	if (rw->readCount == 1)
	{
		pthread_mutex_lock(&rw->lock);
	}
	pthread_mutex_unlock(&rw->readLock);
	pthread_mutex_unlock(&rw->readTry);

	// if (rw->writeCount == 1 || rw->waitingWriters > 0)
	// {
	// 	rw->waitingReaders++;
	// 	pthread_cond_wait(&rw->readLock, &rw->lock);
	// 	rw->waitingReaders--;
	// }
}

// The pthread based reader unlock
void ReaderUnlock(struct read_write_lock *rw)
{
	//	Implement reader unlock
	pthread_mutex_lock(&rw->readLock);
	rw->readCount--;
	if (rw->readCount == 0)
	{
		pthread_mutex_unlock(&rw->lock);
	}
	pthread_mutex_unlock(&rw->readLock);
	// if (rw->waitingWriters > 0)
	// {
	// 	pthread_cond_signal(&rw->writeLock);
	// }
	// else
	// {
	// 	pthread_cond_broadcast(&rw->readLock);
	// }
}

// The pthread based writer lock
void WriterLock(struct read_write_lock *rw)
{
	pthread_mutex_lock(&rw->writeLock);
	rw->writeCount++;
	if (rw->writeCount == 1)
	{
		pthread_mutex_lock(&rw->readTry);
	}
	pthread_mutex_unlock(&rw->writeLock);
	pthread_mutex_lock(&rw->lock);
	// if (rw->readCount > 0 || rw->writeCount == 1)
	// {
	// 	rw->waitingWriters++;
	// 	pthread_cond_wait(&rw->writeLock, &rw->lock);
	// 	rw->waitingWriters--;
	// }

	// rw->writeCount = 1;
}

// The pthread based writer unlock
void WriterUnlock(struct read_write_lock *rw)
{
	pthread_mutex_unlock(&rw->lock);
	pthread_mutex_lock(&rw->writeLock);
	rw->writeCount--;
	if (rw->writeCount == 0)
	{
		pthread_mutex_unlock(&rw->readTry);
	}
	pthread_mutex_unlock(&rw->writeLock);
	// rw->writeCount = 0;
	// if (rw->waitingWriters > 0)
	// {
	// 	pthread_cond_signal(&rw->writeLock);
	// }
	// else
	// {
	// 	pthread_cond_broadcast(&rw->readLock);
	// }
}

//	Call this function to delay the execution by 'delay' ms
void delay(int delay)
{
	struct timespec wait;

	if (delay <= 0)
		return;

	wait.tv_sec = delay / 1000;
	wait.tv_nsec = (delay % 1000) * 1000 * 1000;
	nanosleep(&wait, NULL);
}

// The pthread reader start function
void *ReaderFunction(void *args)
{
	argument_t *arg = (argument_t *)args;
	//	Delay the execution by arrival time specified in the input
	delay(arg->delay);
	//	....
	//  Get appropriate lock
	//	Display  thread ID and value of the shared data variable
	//
	ReaderLock(&rwlock);
	printf("Reader %d: %ld\n", arg->id, data);
	//  Add a dummy delay of 1 ms before lock release
	//	....
	delay(1);
	ReaderUnlock(&rwlock);
}

// The pthread writer start function
void *WriterFunction(void *args)
{
	argument_t *arg = (argument_t *)args;
	//	Delay the execution by arrival time specified in the input
	delay(arg->delay);
	//	....
	//
	//  Get appropriate lock
	//	Increment the shared data variable
	//	Display  thread ID and value of the shared data variable
	//
	WriterLock(&rwlock);
	data++;
	printf("Writer: %d, Data: %ld\n", arg->id, data);
	//  Add a dummy delay of 1 ms before lock release
	//	....
	delay(1);
	WriterUnlock(&rwlock);
}

int main(int argc, char *argv[])
{
	pthread_t *threads;
	argument_t *arg;

	long int reader_number = 0;
	long int writer_number = 0;
	long int thread_number = 0;
	long int total_threads = 0;

	int count = 0; // Number of 3 tuples in the inputs.	Assume maximum 10 tuples
	int rws[10];
	int nthread[10];
	int delay[10];

	//	Verifying number of arguments
	if (argc < 4 || (argc - 1) % 3 != 0)
	{
		printf("reader-writer <r/w> <no-of-threads> <thread-arrival-delay in ms> ...\n");
		printf("Any number of readers/writers can be added with repetitions of above mentioned 3 tuple \n");
		exit(1);
	}

	//	Reading inputs
	for (int i = 0; i < (argc - 1) / 3; i++)
	{
		char rw[2];
		strcpy(rw, argv[(i * 3) + 1]);

		if (strcmp(rw, "r") == 0 || strcmp(rw, "w") == 0)
		{
			if (strcmp(rw, "r") == 0)
				rws[i] = 1; // rws[i] = 1, for reader
			else
				rws[i] = 2; // rws[i] = 2, for writer

			nthread[i] = atol(argv[(i * 3) + 2]);
			delay[i] = atol(argv[(i * 3) + 3]);

			count++;					 //	Number of tuples
			total_threads += nthread[i]; //	Total number of threads
		}
		else
		{
			printf("reader-writer <r/w> <no-of-threads> <thread-arrival-delay in ms> ...\n");
			printf("Any number of readers/writers can be added with repetitions of above mentioned 3 tuple \n");
			exit(1);
		}
	}

	// Initializing reader-writer lock
	InitalizeReadWriteLock(&rwlock);

	//	Create reader/writer threads based on the input and read and write.
	threads = (pthread_t *)malloc(sizeof(pthread_t) * total_threads);
	arg = (argument_t *)malloc(sizeof(argument_t) * total_threads);
	for (int i = 0; i < count; i++)
	{
		if (rws[i] == 1)
		{
			for (int j = 0; j < nthread[i]; j++)
			{
				arg[thread_number].delay = delay[i];
				arg[thread_number].id = reader_number;
				pthread_create(&threads[thread_number], NULL, &ReaderFunction, &arg[thread_number]);
				reader_number++;
				thread_number++;
			}
		}
		else
		{
			for (int j = 0; j < nthread[i]; j++)
			{
				arg[thread_number].delay = delay[i];
				arg[thread_number].id = writer_number;
				pthread_create(&threads[thread_number], NULL, &WriterFunction, &arg[thread_number]);
				writer_number++;
				thread_number++;
			}
		}
	}

	//  Clean up threads on exit
	for (int i = 0; i < total_threads; i++)
		pthread_join(threads[i], NULL);

	// Destroy reader-writer lock
	DestroyReadWriteLock(&rwlock);

	free(threads);
	free(arg);

	exit(0);
}
