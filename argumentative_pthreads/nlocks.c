#include <stdio.h>
#include <pthread.h>
#include <assert.h>

#define NTHREADS 10

long int data[NTHREADS] = {0};
pthread_mutex_t lock[NTHREADS];

void *myThread(void *);

int main()
{
    int arg[NTHREADS];
    // Create space for pthread variables
    pthread_t tid[NTHREADS];

    // Initialize the mutex
    for (int i = 0; i < NTHREADS; i++)
    {
        int rc = pthread_mutex_init(&lock[i], NULL);
        assert(rc == 0);
    }

    for (int i = 0; i < NTHREADS; i++)
    {
        arg[i] = i;
        //	Create a thread with default attributes and no arguments
        pthread_create(&tid[i], NULL, myThread, (void *)&arg[i]);
    }

    for (int i = 0; i < NTHREADS; i++)
    {
        //	Wait for all threads to finish
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i < 1000; i++)
    {
        for (int j = 0; j < NTHREADS; j++)
        {
            data[j]++;
        }
    }

    for (int i = 0; i < NTHREADS; i++)
    {
        printf("Data[%d]: %ld \n", i, data[i]);
    }

    // Destroy the mutex
    for (int i = 0; i < NTHREADS; i++)
    {
        pthread_mutex_destroy(&lock[i]);
    }

    return 0;
}

void *myThread(void *arg)
{
    int i = *((int *)arg);
    for (int j = 0; j < 1000; j++)
    {
        pthread_mutex_lock(&lock[i]);
        data[i]++;
        pthread_mutex_unlock(&lock[i]);
    }
}