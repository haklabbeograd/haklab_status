#!/bin/bash
set -e

auth=""
echo -e "\n* * * CouchDb Initialisation * * *\n"
echo "Enter username:"
read user
if [ $user ]; then
    echo "Enter password:"
    stty -echo
    read pass
    stty echo
    auth=$user:$pass@
fi

echo -n "Initialising CouchDB... "

(
    curl -s -X DELETE http://${auth}localhost:5984/hacklab_status
    curl -s -X PUT http://${auth}localhost:5984/hacklab_status
    curl -s -X POST http://${auth}localhost:5984/hacklab_status \
    -H "Content-Type: application/json" -d '{ "_id": "brava"}'
    curl -s -X POST http://${auth}localhost:5984/hacklab_status \
    -H "Content-Type: application/json" -d '{ "_id": "temperature"}'
    curl -s -X POST http://${auth}localhost:5984/hacklab_status \
    -H "Content-Type: application/json" -d '{ "_id": "humidity"}'
) |\
tee couchdb-init |\
grep error |\
if [ $(wc -l) == "0" ]; then
    echo "OK"
else
    mv couchdb-init couchdb-init.error
    echo -e "ERROR\nView 'couchdb-init.error' for info.\n"
    exit 1
fi
rm -f couchdb-init.error
echo
