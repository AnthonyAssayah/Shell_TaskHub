CC = gcc
FLAGS = -Wall -g

.PHONY: all
all: myshell

myshell: myshell.c myshell.h history.c history.h
	$(CC) $(FLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -rf *.o myshell 