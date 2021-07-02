CC=gcc
CFLAGS=-c -O2 -Wall -g

all: super

super: clean main

main: main.o
	$(CC) main.o -o main -lpthread -lcurl -ltidy

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	/bin/rm -f main *.o *.gz && rm -rf ./output/out.txt

run:
	./main http://www.calendar.ubc.ca/vancouver/courses.cfm?page=name

debug:
	gdb ./main
