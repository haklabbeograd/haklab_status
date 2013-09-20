#!/usr/bin/make
CC=gcc
CFLAGS=-g -Wall -I.
LIBS=$$(curl-config --libs)
OBJS=main.o config.o controller.o couchdb.o

GIT_VERSION=$$(git log --pretty=%h -n1 . 2>/dev/null || echo)
ETCDIR=$(DESTDIR)/etc/haklab-status/

make: version-clean
	$(MAKE) --no-print-directory haklab-status

all: make
	$(MAKE) --no-print-directory couchdb-init

.PHONY: make all run install uninstall openwrt clean-all clean couchdb-clean version-clean

.SILENT: make all version.h openwrt clean couchdb-clean version-clean

main.o: main.c version.h config.h controller.h
config.o: couchdb.h
controller.o: couchdb.h
version.h:
	echo "#define VERSION \"$$(cat VERSION)\"" > version.h
	echo "#define GIT_VERSION \"$(GIT_VERSION)\"" >> version.h

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
	test $${OPENWRT_DIR} || (echo "OpenWRT directory not specified. Run:" && \
	    echo "make openwrt OPENWRT_DIR=/path/to/openwrt/dir" && false)
	rm -rf $(OPENWRT_DIR)/package/haklab-status
	mkdir -p $(OPENWRT_DIR)/package/haklab-status/src
	cp openwrt/Makefile $(OPENWRT_DIR)/package/haklab-status
	cp *.c *.h Makefile VERSION $(OPENWRT_DIR)/package/haklab-status/src
	sed -i s/PKG_VERSION_UNSET/PKG_VERSION:=$$(cat VERSION)/ \
	    $(OPENWRT_DIR)/package/haklab-status/Makefile
	echo $(GIT_VERSION) > $(OPENWRT_DIR)/package/haklab-status/src/GIT_VERSION

version-clean:
	rm -f version.h

clean: version-clean
	rm -f haklab-status
	rm -f *.o

couchdb-clean:
	rm -f couchdb-init couchdb-init.error

clean-all: clean couchdb-clean
