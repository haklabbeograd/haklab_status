#include "couchdb.h"

typedef struct {
    couchdb_doc *doc;
    int docc;
} couchdb_conf;

typedef struct {
    unsigned int baudrate;
    char *serial_port;
    char *controller;
} main_conf;

main_conf main_conf_parse();
void main_conf_clean(main_conf conf);

couchdb_conf couchdb_conf_parse();
void couchdb_conf_clean(couchdb_conf conf);

