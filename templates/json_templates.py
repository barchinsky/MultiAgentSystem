#!/usr/bin/env python

from string import Template

REGISTRATION=Template(u'{"API_KEY":"register","OBJECT":{"ID":$ID}}')

IS_CAN_MOVE=Template(u'"API_KEI":"is_can_move","OBJECT":{"COORD":$COORD,"VECTOR":$VECTOR}')
