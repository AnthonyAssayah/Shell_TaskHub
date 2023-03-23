CC = gcc
FLAGS = -Wall -g

.PHONY: all
all: myshell

myshell: myshell.c
	$(CC) $(FLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -rf *.o myshell