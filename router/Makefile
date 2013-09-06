#!/usr/bin/make
CC=gcc
CFLAGS=-g -Wall -I.
LIBS=$$(curl-config --libs)
OBJS=main.o couchdb.o config.o

make: haklab-status

all: haklab-status couchdb-init

.PHONY: make all haklab-status run install uninstall openwrt clean-all clean couchdb-clean

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

haklab-status: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

run: haklab-status
	./haklab-status

couchdb-init:
	./couchdb-init.sh

install: haklab-status
	mkdir -p /etc/haklab-status
	cp haklab-status /usr/bin/
	cp *.conf.example /etc/haklab-status/

	cp -n main.conf.example /etc/haklab-status/main.conf
	cp -n couchdb.conf.example /etc/haklab-status/couchdb.conf

uninstall:
	rm -rf /usr/bin/haklab-status /etc/haklab-status/

openwrt:
	test $${OPENWRT_DIR}
	rm -rf $(OPENWRT_DIR)/package/haklab-status
	mkdir -p $(OPENWRT_DIR)/package/haklab-status/src
	cp openwrt/Makefile $(OPENWRT_DIR)/package/haklab-status
	cp *.c *.h Makefile $(OPENWRT_DIR)/package/haklab-status/src
	$(OPENWRT_DIR)/scripts/feeds update packages
	$(OPENWRT_DIR)/scripts/feeds install libcurl

clean-all: clean couchdb-clean

clean:
	rm -f haklab-status
	rm -f *.o

couchdb-clean:
	rm -f couchdb-init couchdb-init.error
