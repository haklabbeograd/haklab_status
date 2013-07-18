#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "couchdb.h"

couchdb_doc couchdb_document_init(char *db, char *id) {
    couchdb_doc doc;
    doc.db = strdup(db);
    doc.id = strdup(id);
    doc.rev = 0;
    doc.first_field = 0;
    doc.last_field = 0;

    doc.add_field = couchdb_document_add_field;
    doc.post_revision = couchdb_document_post_revision;
    doc.clean = couchdb_document_clean;

    curl_global_init(CURL_GLOBAL_ALL);
    return doc;
}

void couchdb_document_add_field(couchdb_doc *doc, char *key, char *value) {
    couchdb_field *field;

    field = malloc(sizeof(couchdb_field));
    field->key = key;
    field->value = value;
    field->next = 0;

    if (doc->last_field)
        doc->last_field->next = field;
    else
        doc->first_field = field;
    doc->last_field = field;
}

void couchdb_document_clean(couchdb_doc *doc) {
    if (doc->db) free(doc->db);
    if (doc->id) free(doc->id);
    if (doc->rev) free(doc->rev);
}

struct revdata {
    const char *revstr;
    char **rev;
};

size_t write_data(char *ptr, size_t size, size_t nmemb,
                  struct revdata *userdata) {
    size_t e() {
        fprintf(stderr, "%s\n", ptr);
        return size*nmemb;
    }
    char *needle;
    asprintf(&needle, "\"%s\":\"", userdata->revstr);
    char *start, *end;
    start = strstr(ptr, needle);
    if (!start) return e();
    start += strlen(needle);
    free(needle);
    end = strstr(start, "\"");
    if (!end) return e();
    *end = 0;
    asprintf(userdata->rev, "%s", start);
    return size*nmemb;
}

char *couchdb_document_get_revision(couchdb_doc *doc) {
    char *rev = 0;
    struct revdata rvd = {"_rev", &rev};
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl) return 0;

    char *url;
    asprintf(&url, "%s/%s", doc->db, doc->id);
    curl_easy_setopt(curl, CURLOPT_URL, url);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rvd);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        fprintf(stderr, "Curl failed: %s\n",
                curl_easy_strerror(res));
    free(url);
    curl_easy_cleanup(curl);
    doc->rev = rev;
    return rev;
}

char *couchdb_document_post_revision(couchdb_doc *doc) {
    if (!doc->rev)
        if (!couchdb_document_get_revision(doc))
            return 0;
    char *rev = 0;
    struct revdata rvd = {"rev", &rev};

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (!curl) return rev;
    curl_easy_setopt(curl, CURLOPT_URL, doc->db);

    struct curl_slist *chunk;
    chunk = curl_slist_append(0, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    char *fields, *f, *json;
    couchdb_field *field;
    f = fields = malloc(sizeof(char));
    f[0] = 0;
    if (doc->first_field) {
        field = doc->first_field;
        while (field) {
            asprintf(&fields, "%s,\"%s\":\"%s\"", f, field->key, field->value);
            free(f);
            f = fields;
            doc->last_field = field;
            field = field->next;
            free(doc->last_field);
        }
        doc->first_field = doc->last_field = 0;
    }
    asprintf(&json, "{\"_id\":\"%s\",\"_rev\":\"%s\"%s}",
             doc->id, doc->rev, fields);
    free(fields);

    //printf("Request: %s\n", json);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rvd);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        fprintf(stderr, "Curl failed: %s\n",
                curl_easy_strerror(res));
    else {
        if (rev) {
            free(doc->rev);
            doc->rev = rev;
        }
    }
    free(json);
    curl_slist_free_all(chunk);
    curl_easy_cleanup(curl);
    return rev;
}
