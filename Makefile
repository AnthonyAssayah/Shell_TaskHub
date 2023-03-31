CC = gcc
FLAGS = -Wall -g

.PHONY: all
all: newShell

newShell: newShell.c history.h history.c
	$(CC) $(FLAGS) $< history.c -o $@


.PHONY: clean
clean:
	rm -rf *.o myshell newShell