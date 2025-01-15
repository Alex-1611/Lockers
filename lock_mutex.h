#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <signal.h>
#include <sys/syscall.h>

pid_t gettid(void) {
    return syscall(SYS_gettid);
}

typedef struct thr_node{
    pid_t tid;
    struct thr_node *next;
}thr_node;

typedef struct thr_queue{
    thr_node *front;
    thr_node *rear;
}thr_queue;

thr_queue *thr_init_queue();
bool is_empty(thr_queue *queue);
void enqueue(thr_queue *queue, pid_t tid);
pid_t dequeue(thr_queue *queue);
bool in_queue(thr_queue *queue, pid_t tid);
void delete_tid(thr_queue *queue, pid_t tid);


typedef struct mtx_lock{
    thr_queue *thr_queue; 
}mtx_lock;

mtx_lock *mtx_init();
void mtx_destroy(mtx_lock *mtx);

void acquire(mtx_lock *mtx);
void release(mtx_lock *mtx);

typedef struct semaphore{
    int value;
    thr_queue *thr_queue;
}semaphore;

semaphore *sem_init(int value);
void wait(semaphore *sem);
void signal_sem(semaphore *sem);
void sem_destroy(semaphore *sem);

pid_t nth_in_queue(semaphore *sem);
bool in_first_n_queue(semaphore *sem, pid_t tid);

