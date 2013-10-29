#!/usr/bin/env python

import socket
import ConfigParser
import  time
import sys
import json
import warnings
import math
import random

sys.path.append('templates')
from templates import *

class Ant:
    def __init__(self,exteranl_id):
        self._config=ConfigParser.RawConfigParser()
        self._config.read('conf/config.conf')
        self._host=self._config.get("Ant","host")
        self._port=self._config.getint("Ant","port")
        
        self.client_socket=socket.socket(socket.AF_INET,socket.SOCK_STREAM) # socket connects to anthell server

        self._external_id = exteranl_id
        self._id=int(time.time())
        self._pos_x=0
        self._pos_y=0

        self._passed_way_to_resource=[] #array of passed way (x,y) coordinate to resource dislocation

    def connect(self,host,port):
        try:
            self.client_socket.connect((host,port)) # connect to host
        except Exception,e:
            print self._external_id
            print e
            sys.exit()

    def disconnect(self):
        self.client_socket.close()

    def register(self):
        '''
        Provide client registration
        '''
        reg_query = self.create_reg_query(self._id)
        print "register::Registration query:",reg_query

        self.send(reg_query)
        
    def send(self,query):
        '''
        Send data to server
        '''
        print "%s:send::Query to send:%s"%(self._external_id,query)

        try:
            self.client_socket.sendall(query.encode('utf-8'))
            print "%s:send::Data sended."%self._external_id
            response = self.client_socket.recv(1024)
            print "%s:send::Response goted."%self._external_id

            if response:
                self.process_response(response)
            else:
                print "send::No response goted."
        except Exception,e:
            print e

    def process_response(self,response):
        '''
        Process all responses from server.
        '''
        j = json.loads(response)
        
        api_key = j["API_KEY"]

        if api_key == "registration":
            print "Registration section found."
            print j["OBJECT"]
 
            self._pos_x,self._pos_y = j["OBJECT"]["COORD_ANT"]
            print "Coordinates",self._pos_x, self._pos_y

        elif api_key == "is_ant_can_move":
            print "process_response::Is ant can move method called."
            x,y = j["OBJECT"]["COORD_ANT"]
            print "process_response::New coordinates goted:",x,y
            self._pos_x=x
            self._pos_y=y
            self._passed_way_to_resource.append((x,y))
        elif api_key == "ERROR":
            print "process_response::Error section found."
        elif api_key == "nearest_object":
            pass

    def create_reg_query(self,ant_id):
        '''
        Create registration query.
        '''
        query={"API_KEY":"registration","OBJECT":{}}
        return json.dumps(query)

    def create_is_ant_can_move_query(self,vector_x,vector_y):
        '''
        Generate json is_ant_can_move query.
        '''
        query={"API_KEY":"is_ant_can_move","OBJECT":{"VECTOR":[vector_x,vector_y]} }
        return json.dumps(query)

    def create_get_nearest_object_query(self):
        query={"API_KEY":"nearest_objects","OBJECT":{"COORD":[self._pos_x,self._pos_y],"VISION_RANGE":0.05}}
        return json.dumps(query)

    def get_possible_direction(self):
        '''
        Retrive data from server about possible way
        posible_way - array of possible moving directions
        '''
        rad = random.randint(0,360)
        vector_x = math.cos(rad/K)
        vector_y = math.sin(rad/K)
        self.send(self.create_is_ant_can_move_query(vector_x, vector_y))

    def move(self):
        '''
        Move ant, should send data to server about choosen way
        '''
        self.get_possible_direction()

    def send_passed_way(self):
        '''
        define logic to send data about passed way to another ant
        '''
        pass
    
    def perform_way_optimization(self):
        '''
        Define logic for way optimization, should be defined logic to escape loops
        '''
        pass

    def get_nearest_ants(self):
        pass

    def live(self):
        '''
        Main function, starts socket server and describes ant logic
        '''
        self.connect(self._host,self._port)
        self.register()
        while True:
            self.move()
            print "live::I'm alive!"
            time.sleep(0.1)


# /////////////////////// DEBUG ZONE ///////////////////////////
if __name__=='__main__':
    test_ant = Ant(0)
    
    test_ant.live()
