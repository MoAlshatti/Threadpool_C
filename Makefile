CFLAGS = -Wall -Wextra -Wpedantic
CC = gcc

all:
	$(CC) $(CFLAGS) threadpool.c -o threadpool

build:
	$(CC) $(CFLAGS) threadpool.c -o threadpool
	./threadpool