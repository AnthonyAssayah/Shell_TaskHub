CC = gcc
CFLAGS = -Wall -g

all: myshell

myshell: shell.o
	$(CC) $(CFLAGS) -o myshell shell.o

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c

.PHONY: all clean

clean: 
	rm -f *.o myshell





