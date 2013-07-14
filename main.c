#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>

#include "couchdb.h"

#define BAUDRATE B57600
#define SERIAL_PORT "/dev/ttyACM0"

int main() {
    couchdb_doc brava;
    brava = couchdb_document_init("http://localhost:5984/hacklab_status", "brava");
    couchdb_doc temperature;
    temperature = couchdb_document_init("http://localhost:5984/hacklab_status", "temperature");
    couchdb_doc humidity;
    humidity = couchdb_document_init("http://localhost:5984/hacklab_status", "humidity");

    int fd, working, count;
    struct termios tos;
    char buf[255];

    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror(SERIAL_PORT);
        exit(-1);
    }

    bzero(&tos, sizeof(tos));

    tos.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    tos.c_iflag = IGNPAR;
    tos.c_oflag = 0;
    tos.c_lflag = ICANON;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &tos);

    working = 1;
    printf("Listening...\n");

    char *timestr;
    while (working) {
        count = read(fd, buf, 255);
        buf[count] = 0;
        printf("%s\n", buf);

        if (!strncmp(buf, "Brava\t", 6)) {
            brava.add_field(&brava, "value", buf + 6);
            asprintf(&timestr, "%u", (unsigned int)time(0));
            brava.add_field(&brava, "time", timestr);
            brava.post_revision(&brava);
            free(timestr);
            //printf("Brava revision from update: %s\n", brava.rev);
        }
        if (!strncmp(buf, "Temperature\t", 12)) {
            temperature.add_field(&temperature, "value", buf + 12);
            asprintf(&timestr, "%u", (unsigned int)time(0));
            temperature.add_field(&temperature, "time", timestr);
            temperature.post_revision(&temperature);
            free(timestr);
            //printf("Temperature revision from update: %s\n", temperature.rev);
        }
        if (!strncmp(buf, "Humidity\t", 9)) {
            humidity.add_field(&humidity, "value", buf + 9);
            asprintf(&timestr, "%u", (unsigned int)time(0));
            humidity.add_field(&humidity, "time", timestr);
            humidity.post_revision(&humidity);
            free(timestr);
            //printf("Humidity revision from update: %s\n", humidity.rev);
        }
        if (!strncmp(buf, "OK", 2)) working = 0;
    }

    brava.clean(&brava);

    return 0;
}
