#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "couchdb.h"

size_t parser(char *ptr, size_t size, size_t nmemb, void *userp) {
    if (ptr[0] == '\n') return size * nmemb;
    ptr = strstr(ptr, "\"id\":\"");
    if (!ptr) return size * nmemb;
    ptr += 6;
    char *end = strstr(ptr, "\"");
    end[0] = 0;
    if (!strcmp(ptr, "controller")) {
        puts("Controller sent a command.");
    }
    return size * nmemb;
}

void *run_controller(void *url) {
    couchdb_changes cont_doc = couchdb_changes_init(url, &parser, 0);
    while (1) {
        cont_doc.get_last(&cont_doc);
        if (cont_doc.last == -1)
            sleep(540);
        else
            cont_doc.parse(&cont_doc);
        cont_doc.last = -1;
        sleep(60);
    }
}

void run_controller_thread(char *url) {
    pthread_t controller_thread;
    pthread_create(&controller_thread, 0, run_controller, url);
}
