#include "couchdb.h"

typedef struct {
    couchdb_doc *doc;
    int docc;
} CouchDB_conf;

typedef struct {
    unsigned int BAUDRATE;
    char *SERIAL_PORT;
} Main_conf;

Main_conf main_conf_parse();
void main_conf_clean(Main_conf conf);

CouchDB_conf couchdb_conf_parse();
void couchdb_conf_clean(CouchDB_conf conf);

