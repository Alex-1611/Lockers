#ifndef LOCKERS2_H
#define LOCKERS2_H

#include <stdint.h>
#include <stdatomic.h>

typedef struct mutex {
    _Atomic(uint32_t) lock;
} mutex;

void init(mutex *mtx);
void acquire(mutex *mtx);
void release(mutex *mtx);

typedef struct semaphore {
    _Atomic(uint32_t) value;
    mutex mtx;
} semaphore;

void sem_init(semaphore *sem, int value);
void wait(semaphore *sem);
void post(semaphore *sem);

#endif //LOCKERS2_H