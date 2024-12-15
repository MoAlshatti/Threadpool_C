#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#define THREADS_NUMBER 4

typedef struct {
    pthread_t threads [THREADS_NUMBER];
    pthread_mutex_t mutex;
    pthread_cond_t cond_var;
}threadPool;

threadPool pool = (threadPool){.mutex = PTHREAD_MUTEX_INITIALIZER, .cond_var = PTHREAD_COND_INITIALIZER};  // init cond and mu later


typedef struct {
    void *func_ptr;
    // args (add at some point)
    task *next;
}task;

typedef struct {
    task *front;
    task *rear;

}tasksQue;

tasksQue taskQueue = (tasksQue) {.front = NULL, .rear = NULL};

void EnqueueTask(task *task){
    taskQueue.rear->next = task;
    taskQueue.rear = task;

    if (taskQueue.front == NULL){
        taskQueue.front = task;
    }
}

task *DequeueTask(){
    task *t = taskQueue.front;
    taskQueue.front = taskQueue.front->next;
    return t;
}

bool isEmpty(){
    if (taskQueue.front == NULL && taskQueue.rear == NULL){
        return true;
    }  else {
        return false;
    }
}

void *threadWait(void *args){

    while (true) {

        pthread_mutex_lock(&pool.mutex);
        while (isEmpty){
            pthread_cond_wait(&pool.cond_var,&pool.mutex);
        }

        // call the function .... etc.
        task *t = DequeueTask();

        void* (*func) (void *) = t->func_ptr;

        (*func)(NULL);  // when adding args this should not be null
    }

}

void main (void){
    
}