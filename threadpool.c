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
    bool ret;
}threadPool;

threadPool pool = (threadPool){.mutex = PTHREAD_MUTEX_INITIALIZER, .cond_var = PTHREAD_COND_INITIALIZER, .ret = false};


struct task{
    void *func_ptr;
    // args (add at some point)
    struct task *next;
};
typedef struct task task;

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
        while (isEmpty()){
            pthread_cond_wait(&pool.cond_var,&pool.mutex);
            if (pool.ret) {
                pthread_mutex_unlock(&pool.mutex);
                return NULL;
            }
        }

        // call the function .... etc.
        task *t = DequeueTask();

        void* (*func) (void *) = t->func_ptr;

        (*func)(NULL);  // when adding args this should not be null
    }
    return NULL;
}

void threads_init(){
    pthread_mutex_init(&pool.mutex,NULL);
    pthread_cond_init(&pool.cond_var,NULL);

    for (int i = 0; i < THREADS_NUMBER; i++){
        if (pthread_create(&pool.threads[i],NULL,threadWait,NULL) != 0){    //maybe add args later
            //deal with error
        } 

    }
}

void threads_join(){
    pthread_mutex_lock(&pool.mutex);
    pool.ret = true;
    pthread_mutex_unlock(&pool.mutex);
    for (int i = 0; i < THREADS_NUMBER; i++){
        pthread_cond_broadcast(&pool.cond_var);
        if (pthread_join(pool.threads[i],NULL) != 0){
            //deal with error
        }    
    }
}

void threads_cleanup(){
    pthread_mutex_destroy(&pool.mutex);
    pthread_cond_destroy(&pool.cond_var);
}

int main (void){
    threads_init();

    // add tasks

    // finish the program
    threads_join();
    threads_cleanup();
    return EXIT_SUCCESS;
}