

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 5

// Thread function declaration
void* startThread(void*);


int main(){
    pthread_t tids[NUM_THREADS];
    int status;
    long result;

    for(long i = 0; i < NUM_THREADS; i++){
        status = pthread_create(&tids[i], NULL, startThread, (void*) i);
        printf("Thread %ld created with ID: %ld\n", i, tids[i]);
        if (status != 0)
        {
            perror("ERROR: pthread_create");
            exit(EXIT_FAILURE);
        }
    }
    
    // Wait for all threads to finish
    for (int i=0; i<NUM_THREADS; i++)
    {
        status = pthread_join(tids[i], (void *)&result);
        if (status != 0)
        {
            perror("ERROR: pthread_join");
            exit(EXIT_FAILURE);
        }
        printf("Thread %d returned value %ld\n", i, (long)result);
    }
    
    // Wait for all other attached threads to finish
    pthread_exit(NULL);
    
    return 0;
}

void * startThread(void * arg)
{
    long index = (long) arg;
    
    printf("\tThis is thread %ld\n", index);
    sleep(1);
    printf("\tThread %ld is about to finish\n", index);
    
    pthread_exit( (void *) (index * 2) );
}