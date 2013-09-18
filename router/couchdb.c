#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "couchdb.h"

void couchdb_document_add_field(couchdb_doc *doc, const char *key, const char *value);
void couchdb_document_clean(couchdb_doc *doc);
void couchdb_document_post(couchdb_doc *doc);

void couchdb_changes_get_last(couchdb_changes *chan);
void couchdb_changes_parse(couchdb_changes *chan);
void couchdb_changes_clean(couchdb_changes *chan);

size_t write_errors(char *p, size_t s, size_t n, void *u) {
    if (!strncmp(p, "{\"error\":", 9))
        fprintf(stderr, "%s", p);
    return s * n;
}

couchdb_doc couchdb_document_init(const char *db, const char *id) {
    couchdb_doc doc;
    doc.db = strdup(db);
    doc.id = strdup(id);
    doc.first_field = 0;
    doc.last_field = 0;

    doc.add_field = couchdb_document_add_field;
    doc.post = couchdb_document_post;
    doc.clean = couchdb_document_clean;

    curl_global_init(CURL_GLOBAL_ALL);
    return doc;
}

void couchdb_document_add_field(couchdb_doc *doc, const char *key, const char *value) {
    couchdb_field *field;

    field = malloc(sizeof(couchdb_field));
    field->key = strdup(key);
    field->value = strdup(value);
    field->next = 0;

    if (doc->last_field)
        doc->last_field->next = field;
    else
        doc->first_field = field;
    doc->last_field = field;
}

void couchdb_document_post(couchdb_doc *doc) {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, doc->db);

    struct curl_slist *chunk;
    chunk = curl_slist_append(0, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_errors);

    char *fields, *f, *json;
    couchdb_field *field;
    f = fields = malloc(sizeof(char) * 2);
    f[0] = f[1] = 0;
    if (doc->first_field) {
        field = doc->first_field;
        while (field) {
            asprintf(&fields, "%s,\"%s\":%s", f, field->key, field->value);
            free(f);
            free(field->key);
            free(field->value);
            f = fields;
            doc->last_field = field;
            field = field->next;
            free(doc->last_field);
        }
        doc->first_field = doc->last_field = 0;
    }

    asprintf(&json, "{%s}", fields + 1);
    free(fields);

    printf("Request: %s\n", json);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        fprintf(stderr, "Curl failed: %s\n", curl_easy_strerror(res));
    free(json);
    curl_slist_free_all(chunk);
    curl_easy_cleanup(curl);
}

void couchdb_document_clean(couchdb_doc *doc) {
    free(doc->db);
    free(doc->id);
}

couchdb_changes couchdb_changes_init(
    const char *db,
    size_t (*parser)(char *ptr, size_t size, size_t nmemb, void *arg),
    void *arg) {
    couchdb_changes chan;
    char append_char = index(db, '?') ? '&' : '?';
    asprintf(&chan.db, "%s%c", db, append_char);
    chan.parser = parser;
    chan.arg = arg;
    chan.last = -1;
    chan.get_last = couchdb_changes_get_last;
    chan.parse = couchdb_changes_parse;
    chan.clean = couchdb_changes_clean;

    curl_global_init(CURL_GLOBAL_ALL);
    return chan;
}

void couchdb_changes_get_last(couchdb_changes *chan) {
    size_t seq_writer(char *ptr, size_t size, size_t nmemb, int *seq) {
        ptr = strstr(ptr, "\"last_seq\":");
        if (ptr) sscanf(ptr + 11, "%d", seq);
        return size * nmemb;
    }
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    char *url;
    asprintf(&url, "%slimit=1&descending=true", chan->db);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, seq_writer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chan->last);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        fprintf(stderr, "Curl failed: %s\n",
                curl_easy_strerror(res));
    curl_easy_cleanup(curl);
    free(url);
}

void couchdb_changes_parse(couchdb_changes *chan) {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    char *url;
    asprintf(&url, "%sfeed=continuous&heartbeat=30000&since=%d",
             chan->db, chan->last);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, chan->parser);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chan->arg);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        fprintf(stderr, "Curl failed: %s\n",
                curl_easy_strerror(res));
    curl_easy_cleanup(curl);
    free(url);
}

void couchdb_changes_clean(couchdb_changes *chan) {
    free(chan->db);
}
