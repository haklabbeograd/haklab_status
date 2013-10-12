#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <ctype.h>

#include "config.h"


main_conf main_conf_parse() {
    main_conf conf;
    conf.baudrate = B57600;

    asprintf(&conf.serial_port, "/dev/ttyACM0");
    conf.controller = malloc(sizeof(char));
    conf.controller[0] = 0;

    FILE *f = fopen("/etc/haklab-status/main.conf", "r");
    if (!f) {
        f = fopen("main.conf", "r");
        if (!f) {
            perror("main.conf");
            exit(-1);
        }
    }

    int line = 0;
    char buf[255];
    while (fgets(buf, sizeof(buf), f)) {
        line++;

        char *e, *s = buf;
        s[strlen(s) - 1] = 0;

        /* Remove comments */
        if ((e = index(s, '#'))) *e = 0;
        if ((e = index(s, ';'))) *e = 0;

        /* Trim spaces left */
        while (isspace(*s)) s++;
        if (!*s) continue;

        int len = strlen(s) - 1;
        /* Trim spaces right */
        while (isspace(s[len])) s[len--] = 0;
        if (!*s) continue;

        if ((e = index(s, '='))) {
            *e++ = 0;
            len = strlen(s) - 1;
            while (isspace(s[len])) s[len--] = 0;
            while (isspace(*e)) e++;
            if (*s == 0) {
                fprintf(stderr, "Syntax error in main.conf on line %d.\n", line);
                exit(1);
            }
            if (!strcmp("port", s)) {
                free(conf.serial_port);
                asprintf(&conf.serial_port, "%s", e);
            }
            if (!strcmp("baudrate", s)) {
                if (!strcmp("300", e)) conf.baudrate = B300;
                else if (!strcmp("600", e)) conf.baudrate = B600;
                else if (!strcmp("1200", e)) conf.baudrate = B1200;
                else if (!strcmp("2400", e)) conf.baudrate = B2400;
                else if (!strcmp("4800", e)) conf.baudrate = B4800;
                else if (!strcmp("9600", e)) conf.baudrate = B9600;
                else if (!strcmp("19200", e)) conf.baudrate = B19200;
                else if (!strcmp("38400", e)) conf.baudrate = B38400;
                else if (!strcmp("57600", e)) conf.baudrate = B57600;
                else if (!strcmp("115200", e)) conf.baudrate = B115200;
            }
            if (!strcmp("controller", s)) {
                free(conf.controller);
                asprintf(&conf.controller, "%s", e);
            }
        }
    }
    fclose(f);
    return conf;
}

void main_conf_clean(main_conf conf) {
    free(conf.serial_port);
    free(conf.controller);
}

couchdb_conf couchdb_conf_parse() {
    couchdb_conf conf;
    conf.doc = 0;
    conf.docc = 0;

    FILE *f = fopen("/etc/haklab-status/couchdb.conf", "r");
    if (!f) {
        f = fopen("couchdb.conf", "r");
        if (!f) {
            perror("couchdb.conf");
            exit(-1);
        }
    }

    char *database = 0;
    int line = 0;
    char buf[255];
    while (fgets(buf, sizeof(buf), f)) {
        line++;

        char *e, *s = buf;
        s[strlen(s) - 1] = 0;

        /* Remove comments */
        if ((e = index(s, '#'))) *e = 0;
        if ((e = index(s, ';'))) *e = 0;

        /* Trim spaces left */
        while (isspace(*s)) s++;
        if (!*s) continue;

        int len = strlen(s) - 1;
        /* Trim spaces right */
        while (isspace(s[len])) s[len--] = 0;
        if (!*s) continue;

        if (s[0] == '[') {
            len = strlen(s);
            if (s[--len] != ']') {
                fprintf(stderr, "Error in couchdb.conf on line %d.\n", line);
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

    if (!conf.docc) fprintf(stderr, "Warning: No documents defined.\n");
    return conf;
}

void couchdb_conf_clean(couchdb_conf conf) {
    int i = -1;
    while (++i < conf.docc) conf.doc[i].clean(&conf.doc[i]);
    free(conf.doc);
}
