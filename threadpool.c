#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#define THREADS_NUMBER 4

pthread_t threads [THREADS_NUMBER];

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

void main (void){
    
}