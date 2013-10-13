#!/usr/bin/env python

import socket
import ConfigParser

class Ant:
    def __init__(self,x,y):
        self._config=ConfigParser.RawConfigParser()
        self._config.read('conf/ant.conf')
        self._host=self._config.get("Ant","host")
        self._port=self._config.get("Ant","port")
        
        self.client_socket=socket.socket(socket.AF_INTET,socket.SOCK_STREAM) # socket connects to anthell server

        self._pos_x=x
        self._pos_y=y

        self._passed_way_to_resource=[] #array of passed way (x,y) coordinate to resource dislocation

    def get_possible_way(self):
        '''
        Retrive data from server about possible way
        '''
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
        while True:
            pass
        pass
