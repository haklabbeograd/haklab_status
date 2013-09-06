#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <ctype.h>

#include "config.h"

int main() {
    int fd;
    struct termios tos;

    setlinebuf(stdout);

    Main_conf main_conf = main_conf_parse();
    CouchDB_conf couchdb_conf = couchdb_conf_parse();

    fd = open(main_conf.SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror(main_conf.SERIAL_PORT);
        exit(-1);
    }

    bzero(&tos, sizeof(tos));

    tos.c_cflag = main_conf.BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    tos.c_iflag = IGNPAR | IGNCR;
    tos.c_oflag = 0;
    tos.c_lflag = ICANON;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &tos);

    printf("Listening...\n");

    while (1) {
        char buf[255];
        char timestr[11], *namestr;
        int len;
        int count = read(fd, buf, 255);
        if (!count) break;
        buf[count - 1] = 0;
        printf("%s\n", buf);

        int i = -1;
        while (++i < couchdb_conf.docc) {
            len = strlen(couchdb_conf.doc[i].id);
            if ((!strncmp(buf, couchdb_conf.doc[i].id, len - 1)) && (buf[len] == '\t')) {
                buf[len] = 0;
                asprintf(&namestr, "\"%s\"", buf);
                buf[len] = '\t';
                couchdb_conf.doc[i].add_field(&couchdb_conf.doc[i], "name", namestr);
                free(namestr);

                couchdb_conf.doc[i].add_field(&couchdb_conf.doc[i], "value", buf + len + 1);

                snprintf(timestr, 11, "%d", (int)time(0));
                couchdb_conf.doc[i].add_field(&couchdb_conf.doc[i], "time", timestr);

                couchdb_conf.doc[i].post_revision(&couchdb_conf.doc[i]);
            }
        }
    }

    main_conf_clean(main_conf);
    couchdb_conf_clean(couchdb_conf);

    return 0;
}
