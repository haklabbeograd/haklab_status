#!/bin/sh 
cat /proc/net/arp < /dev/zero | grep br-lan | wc -l | awk '{print $1 = $1 - 1}' > /www/.number.txt
mv /www/.number.txt /www/number.txt                                                

curl --request PUT -g --header "X-ApiKey: umGTpLuz5bphaATCoACWKMc5AgQqgCGvs2OZzXw73FvugTL5" \
--data '{"version": "1.0.0", "datastreams": [{"id": "devices", "current_value": '`cat /www/number.txt`'}]}' \
http://api.xively.com/v2/feeds/127929
