#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#define UNUSED(x) (void)(x)
#define THREADS_NUMBER 4

typedef struct {
    pthread_t threads [THREADS_NUMBER];
    pthread_mutex_t mutex;
    pthread_cond_t cond_var;
    bool ret;
}threadPool;

threadPool pool = (threadPool){.mutex = PTHREAD_MUTEX_INITIALIZER, .cond_var = PTHREAD_COND_INITIALIZER, .ret = false};


struct task{
    void* (*func_ptr)(void*);
    void *args;
    struct task *next;
};
typedef struct task task;

typedef struct {
    task *front;
    task *rear;
    size_t size;

}tasksQue;

tasksQue taskQueue = (tasksQue) {.front = NULL, .rear = NULL, .size = 0};

int EnqueueTask(void* (*func)(void*), void *args){
    task *t = (task*)malloc(sizeof(task));
    if (t == NULL){
        fprintf(stderr,"mallocing a task failed...\n");
        return EXIT_FAILURE;
    }
    t->func_ptr = func;
    t->next = NULL;
    t->args = args;
    if (taskQueue.rear != NULL){
        taskQueue.rear->next = t;
    }
    taskQueue.rear = t;
    if (taskQueue.front == NULL){
        taskQueue.front = t;
    }
    taskQueue.size++;
    pthread_cond_signal(&pool.cond_var);
    return EXIT_SUCCESS;
}

task* DequeueTask(void){
    task *t = taskQueue.front;
    taskQueue.front = taskQueue.front->next;
    if (taskQueue.front == NULL){
        taskQueue.rear = NULL;
    }
    taskQueue.size--;
    return t;
}

bool isEmpty(void){
   return (taskQueue.size == 0);
}

void *threadWait(void *args){
    UNUSED(args);
    while (true) {
        pthread_mutex_lock(&pool.mutex);
        while (isEmpty() && !pool.ret){
            pthread_cond_wait(&pool.cond_var,&pool.mutex);
        }
        if (pool.ret){
            pthread_mutex_unlock(&pool.mutex);
            return NULL;
        }
       task *t = DequeueTask();
       void* (*func)(void*) = t->func_ptr;
        pthread_mutex_unlock(&pool.mutex);

        (*func)(t->args); 
        free(t);
    }
    return NULL;
}

int threads_init(void){
    int err = pthread_mutex_init(&pool.mutex,NULL);
    if (err != 0){
        fprintf(stderr,"failed to initialize mutex, quitting ...\n");
        return EXIT_FAILURE;
    }
    err = pthread_cond_init(&pool.cond_var,NULL);
    if (err != 0){
        fprintf(stderr,"failed to initialize the condition variable, quitting ...\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < THREADS_NUMBER; i++){
        if (pthread_create(&pool.threads[i],NULL,threadWait,NULL) != 0){
            fprintf(stderr,"failed to create thread %d, quitting ...\n",i+1);
            return EXIT_FAILURE;
        } 

    }
    return EXIT_SUCCESS;
}

void threads_join(void){
    pthread_mutex_lock(&pool.mutex);
    pool.ret = true;
    pthread_mutex_unlock(&pool.mutex);

    pthread_cond_broadcast(&pool.cond_var);
    
    for (int i = 0; i < THREADS_NUMBER; i++){
        if (pthread_join(pool.threads[i],NULL) != 0){
            fprintf(stderr,"failed to join thread %d\n",i+1);
        }    
    }
}

void threads_cleanup(void){
    pthread_mutex_destroy(&pool.mutex);
    pthread_cond_destroy(&pool.cond_var);
}
//testing function
void* print_hello(void *arg){
    UNUSED(arg);
    for(int i = 0; i < 5; i++){
        printf("hello %d\n",i);
    }
    return NULL;
}
//testing function
void* print_string(void *arg){
    char *s = (char*) arg;
    printf("%s\n",s);
    return NULL;
}

int main (void){
    int err = threads_init();
    if (err == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    // add tasks
    EnqueueTask(&print_hello,NULL);
    EnqueueTask(&print_string,"wazzaz");
    EnqueueTask(&print_string,"wazzaz");
    EnqueueTask(&print_string,"wazzaz");
    EnqueueTask(&print_string,"wazzaz");
    EnqueueTask(&print_string,"wazzaz");


    //allow some time for tasks to finish
    sleep(3);

    // finish the program
    threads_join();
    threads_cleanup();
    return EXIT_SUCCESS;
}
