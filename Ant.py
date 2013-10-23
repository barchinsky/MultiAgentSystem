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
from json_templates import *

class Ant:
    def __init__(self):
        self._config=ConfigParser.RawConfigParser()
        self._config.read('conf/config.conf')
        self._host=self._config.get("Ant","host")
        self._port=self._config.getint("Ant","port")
        
        self.client_socket=socket.socket(socket.AF_INET,socket.SOCK_STREAM) # socket connects to anthell server

        self._id=int(time.time())
        self._pos_x=None
        self._pos_y=None

        self._passed_way_to_resource=[] #array of passed way (x,y) coordinate to resource dislocation

    def connect(self,host,port):
        self.client_socket.connect((host,port)) # connect to host

    def disconnect(self):
        self.client_socket.close()

    def register(self):
        '''
        Provide client registration
        '''
        reg_query = self.get_reg_query(self._id)
        print "register::Registration query:",reg_query

        self.send(reg_query)
        
    def send(self,query):
        '''
        Send data to server
        '''
        print "send::Query to send:",query

        try:
            self.client_socket.sendall(query.encode('utf-8'))
            print "send::Data sended."
            response = self.client_socket.recv(1024)
            print "send::Response goted."

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
            warnings.warn("process_response::Need to define this part of process_response() method.",Warning)
            x,y = j["OBJECT"]["COORD_ANT"]
            print "process_response::New coordinates goted:",x,y
            self._passed_way_to_resource.append((x,y))
        elif api_key == "ERROR":
            print "process_response::Error section found."

    def get_reg_query(self,ant_id):
        '''
        Create registration query.
        '''
        query={"API_KEY":"registration","OBJECT":{}}
        return json.dumps(query)

    def get_is_ant_can_move_query(self,vector_x,vector_y):
        '''
        Generate json is_ant_can_move query.
        '''
        query={"API_KEY":"is_ant_can_move","OBJECT":{"VECTOR":[vector_x,vector_y]} }
        return json.dumps(query)

    def get_possible_direction(self):
        '''
        Retrive data from server about possible way
        posible_way - array of possible moving directions
        '''
        rad = random.randint(0,360)
        vector_x = 1.#math.cos(rad)
        vector_y = 0.#math.sin(rad)
        self.send(self.get_is_ant_can_move_query(vector_x, vector_y))

    def choose_way(self):
        '''
        Choose moving direction if there are several choices and move ant
        '''
        self.move(choosen_x,choosen_y)
        pass

    def move(self):
        '''
        Move ant, should send data to server about choosen way
        '''
        self.get_possible_direction()

        
        #self._passed_way.append((new_x,new_y))
        pass

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
            time.sleep(1)


# /////////////////////// DEBUG ZONE ///////////////////////////
if __name__=='__main__':
    test_ant = Ant()
    
    print 'test'
    test_ant.live()
