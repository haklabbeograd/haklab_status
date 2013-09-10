#!/usr/bin/make
CC=gcc
CFLAGS=-g -Wall -I.
LIBS=$$(curl-config --libs)
OBJS=main.o couchdb.o config.o

ETCDIR=$(DESTDIR)/etc/haklab-status/

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
	mkdir -p $(DESTDIR)/usr/bin $(ETCDIR)
	install haklab-status $(DESTDIR)/usr/bin/
	cp *.conf.example $(ETCDIR)

	test -f $(ETCDIR)main.conf || cp main.conf.example $(ETCDIR)main.conf
	test -f $(ETCDIR)couchdb.conf || cp couchdb.conf.example $(ETCDIR)couchdb.conf

uninstall:
	rm -rf $(DESTDIR)/usr/bin/haklab-status $(ETCDIR)

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
