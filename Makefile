CC = gcc
FLAGS = -Wall -g

all: myshell

myshell: myshell.c
	$(CC) $(FLAGS) -o myshell myshell.c

.PHONY: clean
clean:
	rm -rf *.o myshell
	