#include "lockers.h"
#include <stdio.h>

void handle_sigcont(int sig){
    if(sig == SIGCONT)
        printf("Received SIGCONT signal\n");
}

thr_queue* thr_init_queue(){
    thr_queue *queue = (thr_queue*)malloc(sizeof(thr_queue));
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}

bool is_empty(thr_queue *queue){
    if(queue->front == NULL)
        return true;
    return false;
}

void enqueue(thr_queue *queue, pid_t tid, pthread_t thread){
    if(!is_empty(queue))
    {   
        thr_node *node = (thr_node*)malloc(sizeof(thr_node));
        node->next = NULL;
        node->tid = tid;
        node->thread = thread;
        queue->rear->next = node;
        queue->rear = node;
    }
    else
    {
        thr_node *node = (thr_node*)malloc(sizeof(thr_node));
        node->next = NULL;
        node->tid = tid;
        node->thread = thread;
        queue->front = node;
        queue->rear = node;
    }
}

pid_t dequeue(thr_queue *queue){
    if(!is_empty(queue)){
        thr_node *node = queue->front;
        pid_t tid = queue->front->tid;
        queue->front = queue->front->next;
        if(queue->front == NULL)
            queue->rear = NULL;
        free(node);
        return tid;
    } 
    else
        return 0;
}

void delete_tid(thr_queue *queue, pid_t tid){
    if(!is_empty(queue)){
        thr_node *node = queue->front;
        thr_node *prev = NULL;
        while(node != NULL)
        {
            if(node->tid == tid)
            {
                if(prev == NULL)
                    queue->front = node->next;
                else
                    prev->next = node->next;
                free(node);
                return;
            }
            prev = node;
            node = node->next;
        }
    }
}

bool in_queue(thr_queue *queue, pid_t tid){
    if(is_empty(queue))
        return false;
    else
    {
        thr_node *node = queue->front;
        while(node != NULL)
        {
            if(node->tid == tid)
                return true;
            else
                node = node->next;
        }
        return false;
    }
}

bool in_first_n_queue(semaphore *sem, pid_t tid){
    if(is_empty(sem->thr_queue))
        return false;
    else
    {
        thr_node *node = sem->thr_queue->front;
        int i = 0;
        while(node != NULL && i < sem->value)
        {
            if(node->tid == tid)
                return true;
            else
            {
                node = node->next;
                i++;
            }
        }
        return false;
    }
}   

pthread_t nth_in_queue(semaphore *sem){
    if(is_empty(sem->thr_queue))
        return 0;
    else
    {
        thr_node *node = sem->thr_queue->front;
        int i = 0;
        while(node != NULL && i < sem->value-1)
        {
            node = node->next;
            i++;
        }
        if(node != NULL && i == sem->value)
            return node->thread;
        else
            return 0;
    }
}

mtx_lock *mtx_init(){
    mtx_lock *mtx = (mtx_lock*)malloc(sizeof(mtx_lock));
    mtx->thr_queue = thr_init_queue();
    return mtx;
}

void mtx_destroy(mtx_lock *mtx){
    while((!is_empty(mtx->thr_queue))){
        pthread_kill((mtx->thr_queue->front->thread), SIGKILL);
        dequeue(mtx->thr_queue);
    }
    free(mtx->thr_queue);
    free(mtx);
}

void acquire(mtx_lock *mtx){
    signal (SIGINT, handle_sigcont);
    pid_t tid = gettid();
    if(!in_queue(mtx->thr_queue, tid))
        enqueue(mtx->thr_queue, tid, pthread_self());
    while(mtx->thr_queue->front->tid != tid)
       sleep(0.1);
}

void release(mtx_lock *mtx){
    pid_t tid = gettid();
    if(!is_empty(mtx->thr_queue) && mtx->thr_queue->front->tid == tid)
        dequeue(mtx->thr_queue);
    if(!is_empty(mtx->thr_queue)){
        pthread_t thread = mtx->thr_queue->front->thread;
        pthread_kill(thread, SIGCONT);
    }
}

semaphore *sem_init(int value){
    semaphore *sem = (semaphore*)malloc(sizeof(semaphore));
    sem->value = value;
    sem->thr_queue = thr_init_queue();
    return sem;
}

void wait(semaphore *sem){
    pid_t tid = gettid();
    if(!in_queue(sem->thr_queue, tid))
        enqueue(sem->thr_queue, tid, pthread_self());
    while(!in_first_n_queue(sem, tid))
        sleep(0.1);
}

void signal_sem(semaphore *sem){
    pid_t tid = gettid();
    delete_tid(sem->thr_queue, tid);
    if(nth_in_queue(sem))
        pthread_kill(nth_in_queue(sem), SIGCONT);
}

void sem_destroy(semaphore *sem){
    while((!is_empty(sem->thr_queue))){
        pthread_kill((sem->thr_queue->front->thread), SIGKILL);
        dequeue(sem->thr_queue);
    }
    free(sem->thr_queue);
    free(sem);
}
