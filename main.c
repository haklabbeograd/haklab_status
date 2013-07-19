#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <ctype.h>

#include "couchdb.h"

#define BAUDRATE B57600
#define SERIAL_PORT "/dev/ttyACM0"

int main() {
    FILE *f;
    char *s, *e;
    int line, len;

    int fd, working, count;
    struct termios tos;
    char buf[255];

    char *database = 0;
    couchdb_doc *doc = 0;
    int docc = 0;

    f = fopen("/etc/haklab-status/couchdb.conf", "r");
    if (!f) {
        f = fopen("couchdb.conf", "r");
        if (!f) {
            perror("couchdb.conf");
            exit(-1);
        }
    }

    void conferror() {
        printf("Error in couchdb.conf on line %d.\n", line);
        exit(1);
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
            if (s[--len] != ']') conferror();
            s[len] = 0;
            if (database) free(database);
            database = strdup(++s);
        } else if (database) {
            doc = realloc(doc, ++docc * sizeof(couchdb_doc));
            doc[docc -1]  = couchdb_document_init(database, s);
        }
    }
    if (database) free(database);
    fclose(f);

    if (!docc) printf("Warning: No documents defined.\n");

    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror(SERIAL_PORT);
        exit(-1);
    }

    bzero(&tos, sizeof(tos));

    tos.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    tos.c_iflag = IGNPAR | IGNCR;
    tos.c_oflag = 0;
    tos.c_lflag = ICANON;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &tos);

    working = 1;
    printf("Listening...\n");

    char *timestr, *namestr;
    int i;
    while (working) {
        count = read(fd, buf, 255);
        buf[count - 1] = 0;
        printf("%s\n", buf);

        for (i = 0; i < docc; i++) {
            len = strlen(doc[i].id);
            if ((!strncmp(buf, doc[i].id, len - 1)) && (buf[len] == '\t')) {
                buf[len] = 0;
                asprintf(&namestr, "\"%s\"", buf);
                doc[i].add_field(&doc[i], "name", namestr);

                doc[i].add_field(&doc[i], "value", buf + len + 1);

                asprintf(&timestr, "%u", (unsigned int)time(0));
                doc[i].add_field(&doc[i], "time", timestr);
                doc[i].post_revision(&doc[i]);

                free(namestr);
                free(timestr);
            }
        }
        if (!strncmp(buf, "EXIT", 5)) working = 0;
    }

    for (i = 0; i < docc; i++) doc[i].clean(&doc[i]);
    free(doc);

    return 0;
}
