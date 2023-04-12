CC = gcc
FLAGS = -Wall -g

.PHONY: all
all: myshell

myshell: myshell.c history.h history.c
	$(CC) $(FLAGS) $< history.c -o $@


.PHONY: clean
clean:
	rm -rf *.o myshell 