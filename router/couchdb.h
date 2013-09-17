typedef struct CouchDB_Document_Field {
    char *key;
    char *value;
    struct CouchDB_Document_Field *next;
} couchdb_field;

typedef struct CouchDB_Document {
    char *db;
    char *id;
    char *rev;
    couchdb_field *first_field;
    couchdb_field *last_field;

    char *(*post_revision)(struct CouchDB_Document *doc);
    void (*add_field)(struct CouchDB_Document *doc, const char *key, const char *value);
    void (*clean)(struct CouchDB_Document *doc);
} couchdb_doc;

couchdb_doc couchdb_document_init(const char *db, const char *id);

typedef struct CouchDB_Changes {
    char *db;
    size_t (*parser)(char *ptr, size_t size, size_t nmemb, void *arg);
    void *arg;
    char *id;
    int last;

    void (*get_last)(struct CouchDB_Changes *chan);
    void (*parse)(struct CouchDB_Changes *chan);
    void (*clean)(struct CouchDB_Changes *chan);
} couchdb_changes;

couchdb_changes couchdb_changes_init(
    const char *db,
    size_t (*parser)(char *ptr, size_t size, size_t nmemb, void *arg),
    void *arg);
