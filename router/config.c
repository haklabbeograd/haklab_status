#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <ctype.h>

#include "config.h"


Main_conf main_conf_parse() {
    Main_conf conf;
    conf.BAUDRATE = B57600;

    asprintf(&conf.SERIAL_PORT, "/dev/ttyACM0");
    FILE *f;
    char *s, *e;
    int line, len;

    char buf[255];

    f = fopen("/etc/haklab-status/main.conf", "r");
    if (!f) {
        f = fopen("main.conf", "r");
        if (!f) {
            perror("main.conf");
            exit(-1);
        }
    }

    line = 0;
    while (fgets(buf, sizeof(buf), f)) {
        line++;
        s = buf;
        s[strlen(s) - 1] = 0;
        if ((e = index(s, '#'))) *e = 0;
        if ((e = index(s, ';'))) *e = 0;

        while (isspace(*s)) s++;
        if (!*s) continue;

        len = strlen(s) - 1;
        while (isspace(s[len])) s[len--] = 0;
        if (!*s) continue;

        if ((e = index(s, '='))) {
            *e++ = 0;
            len = strlen(s) - 1;
            while (isspace(s[len])) s[len--] = 0;
            while (isspace(*e)) e++;
            if (*s == 0) {
                printf("Syntax error in main.conf on line %d.\n", line);
                exit(1);
            }
            if (!strcmp("port", s)) {
                free(conf.SERIAL_PORT);
                asprintf(&conf.SERIAL_PORT, "%s", e);
            }
            if (!strcmp("baudrate", s)) {
                if (!strcmp("300", e)) conf.BAUDRATE = B300;
                if (!strcmp("600", e)) conf.BAUDRATE = B600;
                if (!strcmp("1200", e)) conf.BAUDRATE = B1200;
                if (!strcmp("2400", e)) conf.BAUDRATE = B2400;
                if (!strcmp("4800", e)) conf.BAUDRATE = B4800;
                if (!strcmp("9600", e)) conf.BAUDRATE = B9600;
                if (!strcmp("19200", e)) conf.BAUDRATE = B19200;
                if (!strcmp("38400", e)) conf.BAUDRATE = B38400;
                if (!strcmp("57600", e)) conf.BAUDRATE = B57600;
                if (!strcmp("115200", e)) conf.BAUDRATE = B115200;
            }
        }
    }
    fclose(f);
    return conf;
}

void main_conf_clean(Main_conf conf) {
    free(conf.SERIAL_PORT);
}

CouchDB_conf couchdb_conf_parse() {
    CouchDB_conf conf;
    conf.doc = 0;
    conf.docc = 0;

    FILE *f;
    char *s, *e;
    int line, len;

    char buf[255];

    char *database = 0;
    f = fopen("/etc/haklab-status/couchdb.conf", "r");
    if (!f) {
        f = fopen("couchdb.conf", "r");
        if (!f) {
            perror("couchdb.conf");
            exit(-1);
        }
    }

    line = 0;
    while (fgets(buf, sizeof(buf), f)) {
        line++;
        s = buf;
        s[strlen(s) - 1] = 0;
        if ((e = index(s, '#'))) *e = 0;
        if ((e = index(s, ';'))) *e = 0;

        while (isspace(*s)) s++;
        if (!*s) continue;

        len = strlen(s) - 1;
        while (isspace(s[len])) s[len--] = 0;
        if (!*s) continue;

        if (s[0] == '[') {
            len = strlen(s);
            if (s[--len] != ']') {
                printf("Error in couchdb.conf on line %d.\n", line);
                exit(1);
            }
            s[len] = 0;
            if (database) free(database);
            database = strdup(++s);
        } else if (database) {
            conf.doc = realloc(conf.doc, ++conf.docc * sizeof(couchdb_doc));
            conf.doc[conf.docc -1]  = couchdb_document_init(database, s);
        }
    }
    if (database) free(database);
    fclose(f);

    if (!conf.docc) printf("Warning: No documents defined.\n");
    return conf;
}

void couchdb_conf_clean(CouchDB_conf conf) {
    int i = -1;
    while (++i < conf.docc) conf.doc[i].clean(&conf.doc[i]);
    free(conf.doc);
}
