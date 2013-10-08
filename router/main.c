#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>

#include "version.h"
#include "controller.h"
#include "config.h"

int main(int argc, char *argv[]) {
    for (int argn = 0; ++argn < argc;) {
        if (!strcmp(argv[argn], "--version")) {
            printf("haklab-status %s %s\n", VERSION, GIT_VERSION);
            exit(0);
        }
    }

    int fd;
    struct termios tos;

    setlinebuf(stdout);

    main_conf mconf = main_conf_parse();
    couchdb_conf cconf = couchdb_conf_parse();

    fd = open(mconf.serial_port, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror(mconf.serial_port);
        exit(-1);
    }

    bzero(&tos, sizeof(tos));

    tos.c_cflag = mconf.baudrate | CRTSCTS | CS8 | CLOCAL | CREAD;
    tos.c_iflag = IGNPAR | IGNCR;
    tos.c_oflag = 0;
    tos.c_lflag = ICANON;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &tos);

    if (mconf.controller[0])
        run_controller_thread(mconf.controller, &fd);

    puts("Listening...");
    while (1) {
        char buf[255];
        char timestr[11], *namestr;
        int len;
        int count = read(fd, buf, 255);
        if (!count) break;
        buf[count - 1] = 0;
        printf("%s\n", buf);

        for (int i = -1; ++i < cconf.docc;) {
            len = strlen(cconf.doc[i].id);
            if (!strncmp(buf, cconf.doc[i].id, len - 1) && buf[len] == '\t') {
                buf[len] = 0;
                asprintf(&namestr, "\"%s\"", buf);
                buf[len] = '\t';
                cconf.doc[i].add_field(&cconf.doc[i], "name", namestr);
                free(namestr);

                cconf.doc[i].add_field(&cconf.doc[i], "value", buf + len + 1);

                snprintf(timestr, 11, "%d", (int)time(0));
                cconf.doc[i].add_field(&cconf.doc[i], "time", timestr);

                cconf.doc[i].post(&cconf.doc[i]);
            }
        }
    }

    main_conf_clean(mconf);
    couchdb_conf_clean(cconf);

    return 0;
}
