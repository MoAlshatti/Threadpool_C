# Threadpool_C

A very simple thread pool in c. The idea is that threads are computationally expensive to make, hence a thread pool initializes a predefined number of threads, and keep them waiting for tasks. Taks consist of function pointeres to run, and arguments to these functions. Tasks are organised in a queue. 

# Instructions
1- define your own function
2- enqueue it 

there is a makefile, so
  1- make : compiles the project
  2- make build: compiles and runs the project


# Limitations
I'm using the sleep function to seperate the tasks and joining the threads, which is not really practical and just bad. so you have to adjust the sleep time according to the expected runtime 
of your functions


