# -*- Makefile -*-
CC=gcc
CFLAGS=-I. -lpthread -lcurl -ltidy
MONGOCONFIG=$$(pkg-config --libs --cflags libmongoc-1.0)
DEPS=subject_page_scraper.h index_page_scraper.h

all: main

main: main.c subject_page_scraper.o index_page_scraper.o tpool.o
	$(CC) main.c subject_page_scraper.o index_page_scraper.o tpool.o -o main $(CFLAGS) $(MONGOCONFIG)

subject_page_scraper.o: subject_page_scraper.c subject_page_scraper.h
	$(CC) -c subject_page_scraper.c $(MONGOCONFIG)

index_page_scraper.o: index_page_scraper.c index_page_scraper.h
	$(CC) -c index_page_scraper.c $(MONGOCONFIG)

tpool.o: tpool.c tpool.h
	$(CC) -c tpool.c

clean:
	/bin/rm -f main *.o *.gz
