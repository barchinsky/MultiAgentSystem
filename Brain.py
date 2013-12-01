#!/usr/bin/env python

import math
import Geometry

class Brain:
    def __init__(self):
        self._pos_x=0
        self._pos_y=0        
        self.main_direction_angle = 0
        self._main_direction_setted = False
        self._is_moving_back = False
        self._is_food_found = False
        self.__is_ant_stuck = False
        self._passed_way=[]
        self._new_way = []
        self._bypass_way = []
        self._success_way_distance=1000000000000000.0

    def setPosition(self,x,y):
        self._is_ant_stuck = False
        self._pos_x = x
        self._pos_y = y

        if not self._is_food_found:
            self._passed_way.append([x,y])

    def antStucked(self):
        self.__is_ant_stuck = True

        # direction = random.uniform(-1,1)
        # self._direction_angel -= random.randint(60,100)*[-1,direction][bool(direction)]

    def foodFound(self):
        self._is_food_found = True
        self._success_way_distance = self.find_way_length( self._passed_way )

        self._is_moving_back = True
                
        pass

    def setBarriers(self,barriers):
        if self._is_ant_stuck == True:
            #TODO: calculate bypass way
        pass

    def getNextStepInfo(self):
        pass

    def getNextDirection(self):
        if len(self._bypass_way) == 0:
            if not self._main_direction_setted or self._is_ant_stuck:
                self.main_direction_angle = random.randint(0,360) * math.pi / 180.0
                self._main_direction_setted = True

            deflection_direction = (random.randint(0,10) - 5 )* math.pi / 180.0
            result = self._main_direction_setted + deflection_direction 
        pass
        

    def getNextPosition(self):
        pass

    def find_way_length(self,way):        
        way_len = 0
        for i in xrange(len(way)-1):
            cur_point = way[i]
            next_point = way[i+1]
            way_len += math.sqrt(pow(next_point[0]-cur_point[0],2)+pow(next_point[1]-cur_point[1],2))
        return way_len



        
