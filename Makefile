CC = gcc
FLAGS = -Wall -g

all: myshell newShell

newShell: newShell.c
	$(CC) $(FLAGS) -o newShell newShell.c

myshell: myshell.c
	$(CC) $(FLAGS) -o myshell myshell.c

.PHONY: clean
clean:
	rm -rf *.o myshell
	