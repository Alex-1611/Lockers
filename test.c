
#include "lockers.c"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 5

// Resursa partajată
typedef struct {
    int value;
} shared_resource;

shared_resource resource;

// Mutex pentru sincronizare
mtx_lock *mutex;

// Semafor pentru controlul accesului
semaphore *sem;

void *thread_func_mutex(void *arg) {
    int thread_id = *(int *)arg;
    acquire(mutex);

    printf("Thread %d a intrat in zona critica.\n", thread_id);
    resource.value += 1;
    printf("Thread %d a incrementat valoarea resursei la %d.\n", thread_id, resource.value);
    sleep(1);
    printf("Thread %d iese din zona critica.\n", thread_id);

    release(mutex);
    return NULL;
}

void *thread_func_semaphore(void *arg) {
    int thread_id = *(int *)arg;
    wait(sem);

    printf("Thread %d a intrat in sectiunea permisa de semafor.\n", thread_id);
    sleep(1);
    printf("Thread %d iese din sectiunea permisa de semafor.\n", thread_id);

    signal_sem(sem);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Inițializare resursă partajată
    resource.value = 0;

    // Inițializare mutex
    mutex = mtx_init();

    // Testare mutex
    printf("\nTestare mutex:\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func_mutex, &thread_ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    mtx_destroy(mutex);

    // Inițializare semafor
    sem = sem_init(2); // Permite accesul simultan a 2 fire de execuție

    // Testare semafor
    printf("\nTestare semafor:\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func_semaphore, &thread_ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    sem_destroy(sem);

    return 0;
}