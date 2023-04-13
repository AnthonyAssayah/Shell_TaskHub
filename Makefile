CC = gcc
FLAGS = -Wall -g

.PHONY: all
all: myshell

myshell: myshell.c history.h history.c
	$(CC) $(FLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -rf *.o myshell 