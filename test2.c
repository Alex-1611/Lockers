#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lockers2.c" // Include implementarea ta

#define NUM_THREADS 5
#define NUM_ITERATIONS 10

int shared_resource = 0;
mutex mtx;
semaphore sem;

void *test_mutex(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        acquire(&mtx);
        int tmp = shared_resource;
        usleep(100); // Simulează o întârziere
        shared_resource = tmp + 1;
        printf("Thread %ld incremented shared_resource to %d (mutex)\n", (long)arg, shared_resource);
        printf("Thread %ld exiting critical section (mutex)\n", (long)arg);
        release(&mtx);
    }
    return NULL;
}

void *test_semaphore(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        wait(&sem);
        printf("Thread %ld accessed the resource (semaphore)\n", (long)arg);
        usleep(100); // Simulează o utilizare a resursei
        printf("Thread %ld exiting critical section (semaphore)\n", (long)arg);
        post(&sem);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    // Testare Mutex
    printf("=== Testing Mutex ===\n");
    init(&mtx);
    shared_resource = 0;

    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, test_mutex, (void *)i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final value of shared_resource (mutex): %d\n", shared_resource);

    // Testare Semaphore
    printf("\n=== Testing Semaphore ===\n");
    sem_init(&sem, 2); // Permite accesul a 2 fire simultan

    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, test_semaphore, (void *)i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Semaphore test completed.\n");
    return 0;
}