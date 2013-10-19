#!/usr/bin/env python

import socket
import ConfigParser
import  time
import sys
import json

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
        print "Registration query:",reg_query

        try:
            self.client_socket.sendall(reg_query.encode('utf-8'))
            print "Data sended."
            response = self.client_socket.recv(1024)
            print "Response goted:",response
        except Exception,e:
            print e

    def get_reg_query(self,ant_id):
        register_query=[{"API_KEY":"register","OBJECT":{"ID":and_id}}]
        return json.dumps(register_query)
        return 

    def get_possible_direction(self):
        '''
        Retrive data from server about possible way
        posible_way - array of possible moving directions
        '''
        posible_way=self.server.get_posible_way(self.ant_id)
        pass

    def choose_way(self):
        '''
        Choose moving direction if there are several choices and move ant
        '''
        self.move(choosen_x,choosen_y)
        pass

    def move(self,new_x,new_y):
        '''
        Move ant, should send data to server about choosen way
        '''
        self._passed_way.append((new_x,new_y))
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
        self.connect((self._host,self._port))
        self.register()
        while True:
            print "I'm alive!"
            time.sleep(10)
            print "By!"
            self.disconnect()
            break


# /////////////////////// DEBUG ZONE ///////////////////////////
if __name__=='__main__':
    test_ant = Ant()
    #test_ant.register()
    test_ant.live()
