#include "lockers2.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <linux/futex.h>

void init(mutex *mtx) {
    atomic_init(&mtx->lock, 0);
}

void acquire(mutex *mtx) {
    int expected = 0;
    while (!atomic_compare_exchange_strong(&mtx->lock, &expected, 1)){
        expected = 0;
        syscall(SYS_futex, &mtx->lock,FUTEX_WAIT, 1);
    }
}

void release(mutex *mtx) {
    atomic_store(&mtx->lock, false);
    syscall(SYS_futex, &mtx->lock,FUTEX_WAKE, 1);
}


void sem_init(semaphore *sem, int value) {
    sem->value = value;
    init(&sem->mtx);
}

void wait(semaphore *sem) {
    while(true){
        acquire(&sem->mtx);
        if(sem->value > 0){
            sem->value--;
            release(&sem->mtx);
            break;
        }
        release(&sem->mtx);
        syscall(SYS_futex, &sem->value,FUTEX_WAIT, 1);
    }
}

void post(semaphore *sem) {
    acquire(&sem->mtx);
    sem->value++;
    release(&sem->mtx);
    syscall(SYS_futex, &sem->value, FUTEX_WAKE, 1);
}