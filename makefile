CC = gcc
CFLAGS = -Wall -g

all: shell

shell: shell.o
	$(CC) $(CFLAGS) -o shell shell.o

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c

.PHONY: clean

clean: 
	rm -f *.o shell





