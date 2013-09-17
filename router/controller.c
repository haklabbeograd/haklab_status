#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>

#include "couchdb.h"

struct cont_args {char *url; int *fd;};

size_t write_command(char *ptr, size_t size, size_t nmemb, int *fd) {
    ptr = strstr(ptr, "\"command\":\"");
    if (!ptr) return size * nmemb;
    ptr += 11;
    char *end = rindex(ptr, '"');
    if (!end) return size * nmemb;
    *end = 0;
    char *p = end = ptr;
    while (*end) {
        if (*end == '\\') {
            end++;
            if (!*end) break;
            switch(*end) {
                case ('n'): *p = '\n'; break;
                case ('r'): *p = '\r'; break;
                case ('t'): *p = '\t'; break;
                default: *p = *end;
            }
        } else
            *p = *end;
        p++;
        end++;
    }
    *p = 0;
    write(*fd, ptr, strlen(ptr));
    write(*fd, "\n", 1);
    return size * nmemb;
}

void get_controller_command(char *revision, void *vargs) {
    struct cont_args *args = vargs;
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    char *url;
    asprintf(&url, "%s/controller?rev=%s", args->url, revision);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_command);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, args->fd);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        fprintf(stderr, "Curl failed: %s\n", curl_easy_strerror(res));
    curl_easy_cleanup(curl);
    free(url);
}

size_t parser(char *ptr, size_t size, size_t nmemb, void *args) {
    if (*ptr == '\n') return size * nmemb;
    ptr = strstr(ptr, "\"id\":\"");
    if (!ptr) return size * nmemb;
    ptr += 6;
    char *end = strstr(ptr, "\"");
    if (!end) return size * nmemb;
    *end = 0;
    if (!strcmp(ptr, "controller")) {
        ptr = strstr(end + 1, "\"rev\":\"");
        if (!ptr) return size * nmemb;
        ptr += 7;
        end = strstr(ptr, "\"");
        if (!end) return size * nmemb;
        *end = 0;
        get_controller_command(ptr, args);
    }
    return size * nmemb;
}

void *run_controller(void *vargs) {
    struct cont_args *args = vargs;
    char *url;
    asprintf(&url, "%s/_changes", args->url);
    couchdb_changes cont_doc = couchdb_changes_init(url, &parser, args);
    free(url);
    while (1) {
        cont_doc.get_last(&cont_doc);
        if (cont_doc.last == -1)
            sleep(540);
        else
            cont_doc.parse(&cont_doc);
        cont_doc.last = -1;
        sleep(60);
    }
    cont_doc.clean(&cont_doc);
    free(vargs);
    return 0;
}

void run_controller_thread(char *url, int *fd) {
    struct cont_args *args;
    args = malloc(sizeof(struct cont_args));
    args->url = url;
    args->fd = fd;
    pthread_t controller_thread;
    pthread_create(&controller_thread, 0, run_controller, args);
}
