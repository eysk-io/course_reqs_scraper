CC=gcc
CFLAGS=-c -O2 -Wall -g

all: super

super: clean main

main: main.o
	$(CC) main.o -o main -lpthread -lcurl -ltidy

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	/bin/rm -f main *.o *.gz

run:
	./main

debug:
	gdb ./main
