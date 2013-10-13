#!/usr/bin/env python

from json_templates import *
import json

id=65
data=REGISTRATION.safe_substitute(ID=id)
json_test = json.dumps(data)
print str(json_test)

weird_json ='{"API_KEY":"register","OBJECT":{"ID":id}}'
print json.dumps(weird_json, separators=(',',':'))

