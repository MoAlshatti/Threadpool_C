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
    size_t size;

}tasksQue;

tasksQue taskQueue = (tasksQue) {.front = NULL, .rear = NULL, .size = 0};

void EnqueueTask(void *func){   //could make it take args later
    task *t = (task*)malloc(sizeof(task));
    if (t == NULL){
        //deal with error
    }

    t->func_ptr = func;
    t->next = NULL;
    if (taskQueue.rear != NULL){
        taskQueue.rear->next = t;
    }
    taskQueue.rear = t;
    if (taskQueue.front == NULL){
        taskQueue.front = t;
    }
    taskQueue.size++;
    pthread_cond_broadcast(&pool.cond_var);
}

void* DequeueTask(){
    task *t = taskQueue.front;
    void* func = t->func_ptr;
    taskQueue.front = taskQueue.front->next;
    if (taskQueue.front == NULL){
        taskQueue.rear = NULL;
    }
    taskQueue.size--;
    free(t);
    return func;
}

bool isEmpty(){
   return (taskQueue.size == 0);
}

void *threadWait(void *args){
    while (true) {
        pthread_mutex_lock(&pool.mutex);
        while (isEmpty() && !pool.ret){
            pthread_cond_wait(&pool.cond_var,&pool.mutex);
        }
        if (pool.ret){
            pthread_mutex_unlock(&pool.mutex);
            return NULL;
        }

        // call the function .... etc.
        void* (*func)() = DequeueTask();

        (*func)();  // when adding args this should not be empty
        pthread_mutex_unlock(&pool.mutex);
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

    pthread_cond_broadcast(&pool.cond_var);
    
    for (int i = 0; i < THREADS_NUMBER; i++){
        if (pthread_join(pool.threads[i],NULL) != 0){
            //deal with error
        }    
    }
}

void threads_cleanup(){
    pthread_mutex_destroy(&pool.mutex);
    pthread_cond_destroy(&pool.cond_var);
}
//testing function
void* print_hello(){
    for(int i = 0; i < 5; i++){
        printf("hello %d\n",i);
    }
    return NULL;
}

int main (void){
    threads_init();

    // add tasks
    EnqueueTask(print_hello);


    // finish the program
    threads_join();
    threads_cleanup();
    return EXIT_SUCCESS;
}