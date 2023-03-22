CC = gcc
FLAGS = -Wall -g

all: myshell

myshell: myshell.c
	$(CC) $(FLAGS) $^ -o $@

clean:
	rm -rf myshell