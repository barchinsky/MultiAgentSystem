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
import threading

class Server(threading.Thread):
    def __init__(self):
        super(Server,self).__init__()
        self.host = 'localhost'
        self.port = 65002

        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def run(self):
        self.socket.bind((self.host,self.port))
        self.socket.listen(1)
        conn, addr = self.socket.accept()
        while True:
            data = conn.recv(1024)
            print data
            
        conn.close()


class Ant(threading.Thread):
    def __init__(self,exteranl_id):
        super(Ant,self).__init__()
        self._config=ConfigParser.RawConfigParser()
        self._config.read('conf/config.conf')
        self._host=self._config.get("Ant","host")
        self._port=self._config.getint("Ant","port")
        
        self.client_socket=socket.socket(socket.AF_INET,socket.SOCK_STREAM) # socket connects to anthell server

        self._external_id = exteranl_id
        self._id=int(time.time())
        self._pos_x=0
        self._pos_y=0

        self._passed_way=[] # array of passed way (x,y) coordinate to resource dislocation
        self._success_way=[] # keep all successful ways

        self.server = Server()
 
    def connect(self,host,port):
        print "Connecting..."
        try:
            self.client_socket.connect((host,port)) # connect to host
            print "Connected."
        except Exception,e:
            print "Connection failed:",str(e)
            
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
        print "send::Query to send:%s"%query

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
            print "send::"+str(e)

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
            self._passed_way.append((x,y))
        elif api_key == "ERROR":
            print "process_response::Error section found."
        elif api_key == "nearest_objects":
            print "Nearest object response goted.\n"
            barriers = j["OBJECT"]["BARRIERS"]
            objects = j["OBJECT"]
            foods = j["OBJECT"]["FOODS"]
            print 10*"-",barriers, objects, foods, 10*"-"
            if foods:
                self._success_way.append(self._passed_way)
                raw_input("Continue?")
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
        '''
        Create query for retrieving list of nearest objects.
        '''
        #query={"API_KEY":"nearest_objects","OBJECT":{"COORD":[self._pos_x,self._pos_y],"VISION_RANGE":0.05}}
        print  "Get nearest object request sended."
        query={"API_KEY":"nearest_objects","OBJECT":{}}
        return json.dumps(query)

    def get_possible_direction(self):
        '''
        Retrive data from server about possible way
        posible_way - array of possible moving directions
        '''
        rad = random.randint(0,360)

        vector_x = math.cos(K*180/4)
        vector_y = math.sin(K*180/4)
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
            self.send(self.create_get_nearest_object_query())
            print "live::I'm alive!"
            time.sleep(1)

    def run(self):
        self.server.start()
        self.live()


# /////////////////////// DEBUG ZONE ///////////////////////////
if __name__=='__main__':
    test_ant = Ant(0)
    #test_ant.live()
    test_ant.start()
    test_ant.join()
