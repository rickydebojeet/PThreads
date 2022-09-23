#include <stdio.h>
#include <pthread.h>
#include <assert.h>

#define NTHREADS 100

long int counter = 0;
pthread_mutex_t lock;

//	The thread starter function
void *myThread()
{
    for (int i = 0; i < NTHREADS; i++)
    {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }
}

int main()
{
    // Create space for pthread variables
    pthread_t tid[NTHREADS];

    // Initialize the mutex
    int rc = pthread_mutex_init(&lock, NULL);
    assert(rc == 0);

    for (int i = 0; i < NTHREADS; i++)
    {
        //	Create a thread with default attributes and no arguments
        pthread_create(&tid[i], NULL, myThread, NULL);
    }

    for (int i = 0; i < NTHREADS; i++)
    {
        //	Wait for all threads to finish
        pthread_join(tid[i], NULL);
    }

    printf("Counter: %ld \n", counter);

    // Destroy the mutex
    pthread_mutex_destroy(&lock);

    return 0;
}