#!/usr/bin/make
CC=gcc
CFLAGS=-g -Wall -I. $$(curl-config --cflags)
LIBS=$$(curl-config --libs)

make: haklab-status

all: haklab-status couchdb-init

.PHONY: make all haklab-status run install uninstall clean-all clean couchdb-clean

haklab-status:
	$(CC) main.c couchdb.c -o haklab-status $(LIBS) $(CFLAGS)

run: haklab-status
	./haklab-status

couchdb-init:
	./couchdb-init.sh

install: haklab-status
	mkdir -p /etc/haklab-status
	cp haklab-status /usr/bin/
	cp couchdb.conf.example /etc/haklab-status/
	touch /etc/haklab-status/couchdb.conf

uninstall:
	rm -rf /usr/bin/haklab-status /etc/haklab-status/

clean-all: clean couchdb-clean

clean:
	rm -f haklab-status

couchdb-clean:
	rm -f couchdb-init couchdb-init.error
