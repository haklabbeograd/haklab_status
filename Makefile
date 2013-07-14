#!/usr/bin/make
CC=gcc
CFLAGS=-g -Wall -I. $$(curl-config --cflags)
LIBS=$$(curl-config --libs)

make: haklab-status

all: haklab-status couchdb-init

.PHONY: make all haklab-status run clean-all clean couchdb-clean

haklab-status:
	$(CC) main.c couchdb.c -o haklab-status $(LIBS) $(CFLAGS)

run: haklab-status
	./haklab-status

couchdb-init:
	./couchdb-init.sh

clean-all: clean couchdb-clean

clean:
	rm -f haklab-status

couchdb-clean:
	rm -f couchdb-init couchdb-init.error
