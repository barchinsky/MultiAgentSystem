#!/usr/bin/env python

from json_templates import *
import json

id=65
data=REGISTRATION.safe_substitute(ID=id)
json_test = json.dumps(data)
print str(json_test)

register_query=[{"API_KEY":"register","OBJECT":{"ID":id}}]
json_test2=json.dumps(register_query)
print "test2",json_test2
