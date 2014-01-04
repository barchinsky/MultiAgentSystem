#!/usr/bin/env python

import math
import random
from Geometry import *
import time

def degree_to_radian(degree):
    return degree * math.pi / 180.0

def radian_to_degree(radian):
    return radian * 180.0 / math.pi

class MovingState:    
    searching = 0
    stucked = 1
    bypassing_moving = 2
    go_to_home = 3
    go_to_food = 4
    finding_new_piece_of_food = 5
    new_piece_of_food_founded = 6

class Interlocutor(object):
    def __init__(self,socket):
        self._socket = socket
        self._last_speek_time = time.time()
        print "SPEAK TIME ",self._last_speek_time

    def isCanSpeakAgain(self):
        current_time = time.time()
        diff_time = current_time - self._last_speek_time
        print "diff time ",diff_time
        if diff_time > 15:
            return True

        return False

    def update(self):
        self._last_speek_time = time.time()

    def equal(self,obj):
        return (self._socket == obj._socket)

       

class Brain(object):
    kPosition = 0
    kDirection = 1

    def __init__(self):
        self.observers = []        
        self._position = Point(0,0)  

        self._previus_moving_state = self._current_moving_state = MovingState.searching  
        self._main_direction_angle =degree_to_radian(random.randint(0,360))# degree_to_radian(10) #

        self._is_food_found = False        
        self._passed_way_index = 0      
        self._passed_way=[]
        self._temp_passed_way_to_new_food = []

        self._new_way = []
        self._new_way_length = 100000000000000000.0

        self._barrier = []
        self._bypass_way = []
        self._bypass_way_sector = 0

        self._success_way_distance=1000000000000000.0
        self._step_length = 0.0

        self._lastTimeSpeaking = 0
        self._interlocutors = []

    def attach(self, observer):
        self.observers.append(observer)

    def setPosition(self,x,y):        
        newPos = Point(x,y)
        
        if not self._position.equal(newPos):            
            self._position = newPos            
            if not self._is_food_found:
                self._passed_way.append([x,y])

            if not self.isPointEqualToLastPointInPassedWay([x,y]):
                if (self._current_moving_state == MovingState.finding_new_piece_of_food) or \
                 (self._previus_moving_state == MovingState.finding_new_piece_of_food and \
                    self._current_moving_state == MovingState.bypassing_moving):

                    print "\ntest self._temp_passed_way_to_new_food.append([x,y])"
                    self._temp_passed_way_to_new_food.append([x,y])

    def setMovingState(self,movingState,previes_moving_state = -1):
        if previes_moving_state == -1:            
            self._previus_moving_state = self._current_moving_state
        else:
            print "SET PREVIUS MOVING STATE NOT CURRENT: "
            self._previus_moving_state = previes_moving_state

        self._current_moving_state = movingState

    def antStucked(self):   
        print "ant stucked"  
        self.setMovingState(MovingState.stucked)

    def foodFound(self):
        print "foodFound"

        self._passed_way_index = len(self._passed_way) - 1

        if not self._is_food_found:            
            self._is_food_found = True
            self._success_way_distance = self.find_way_length( self._passed_way )

            if self._current_moving_state != MovingState.go_to_home:
                self.setMovingState(MovingState.go_to_home)                
        else:
            if self._current_moving_state != MovingState.new_piece_of_food_founded:
                self.setMovingState(MovingState.new_piece_of_food_founded)             
        

    def food_brought_to_home(self):
        self.setMovingState(MovingState.go_to_food)
        self._passed_way_index = 1

    def setBarriers(self,barriers):     
        print "setBarriers"  
        if not self._current_moving_state == MovingState.bypassing_moving:            
            for tmp_barrier_coords in barriers:
                barrier = Polygon(tmp_barrier_coords)
                if barrier.isRayIntersect(self._position,self._main_direction_angle):
                    print "barrier detected"
                    self._barrier = barrier
                    return 

    def isCanSpeak(self):
        current_time = time.time()
        diff = current_time - self._lastTimeSpeaking
        if diff > 15:
            return True
        return False   

    def isCanSpeakWithInterlocutor(self,socket):
        print "isCanSpeakWithInterlocutor ",socket
        newInter = Interlocutor(socket)
        isNew = True
        # check if new 
        for tmpInter in self._interlocutors:
            if tmpInter.equal(newInter):
                isNew = False
                newInter = tmpInter
                break

        print "isNew ",isNew
        if isNew:
            self._interlocutors.append(newInter)
            return True
        else:
            isCanSpeak = tmpInter.isCanSpeakAgain()
            print "isCanSpeak ",isCanSpeak
            tmpInter.update()
            return isCanSpeak

        return False

    def updateSelfSpeakingTime(self):
        self._lastTimeSpeaking = time.time()


    def checkNewWayUseful(self,way_length,way_coords):
        print "checkNewWayUseful"
        if len(self._new_way) == 0:            
            if way_length < self._success_way_distance:
                print "USEFUL"
                self._new_way_length = way_length
                self._new_way = way_coords
        else:
            if way_length < self._new_way_length:
                print "USEFUL"
                self._new_way_length = way_length
                self._new_way = way_coords


    def changePassedWayIfNeed(self):
        print "changePassedWayIfNeed"
        if len(self._new_way) > 0:
            print "changed PassedWay"
            self._passed_way = self._new_way
            self._success_way_distance = self._new_way_length
            
            self._new_way_length = 0
            self._new_way = []

    def isPointEqualToLastPointInPassedWay(self,point):
        if len(self._passed_way) == 0:
            return False

        x1,y1 = self._passed_way[-1]
        x2,y2 = point

        if x1 == x2 and y1 == y2:
            return True

        return False


    #################################################################
    ########   NEXT STEP INFO       #################################
    #################################################################

    def getNextStepInfo(self):        
        
        if self._current_moving_state == MovingState.searching: 
            return self.getSearchingDirection()
        elif self._current_moving_state == MovingState.stucked: 
            return self.getStuckedDirection()
        elif self._current_moving_state == MovingState.bypassing_moving: 
            return self.getBypassWayDirection()
        elif self._current_moving_state == MovingState.go_to_home: 
            return self.getGoHomeWayDirection()
        elif self._current_moving_state == MovingState.go_to_food: 
            return self.getGoToFoodWayDirection()
        elif self._current_moving_state == MovingState.finding_new_piece_of_food: 
            return self.getGoFindingNewPieceOfFoodDirection()
        elif self._current_moving_state == MovingState.new_piece_of_food_founded: 
            return self.getGoNewfoodFounded()

        info = {Brain.kDirection: [1 / 2**0.5,1 / 2**0.5]}
        return info

    def getSearchingDirection(self):
        print "getSearchingDirection"
        deflection_direction = degree_to_radian((random.randint(0,20) - 10 ))
        result_rad = self._main_direction_angle + deflection_direction
        vector = Vector()
        vector.setAngleAndLength(result_rad)
        return {Brain.kDirection: vector.getArr()}

    def getStuckedDirection(self):
        print "getStuckedDirection"
        if not self._barrier:            
            self._main_direction_angle = degree_to_radian(random.randint(0,360))
            vector = Vector()
            vector.setAngleAndLength(self._main_direction_angle)
            self.setMovingState(self._previus_moving_state)
            return {Brain.kDirection: vector.getArr()}
        else:
            is_founded = self.find_bypass_way()
            print "is_founded: ",is_founded
            if not is_founded:
                self._barrier = None
                self._bypass_way = []
                self._bypass_way_sector = 0
            else:
                print "\n\nCHECK POINT: previus",self._previus_moving_state
                self.setMovingState(MovingState.bypassing_moving,self._previus_moving_state)
                print "self position"
                self._position.log()
                print "direction ",self._main_direction_angle * 180.0 / math.pi,"\n\n"
                for point in self._bypass_way:
                    point.log()
            return None

    def getBypassWayDirection(self):
        print "getBypassWayDirection"
        count_of_bypass_points = len(self._bypass_way)
        if count_of_bypass_points == 0:
            print "\n\n\nSHOULD NOT SHOW THIS ERROR!!!!\n\n\n "
            self.setMovingState(MovingState.stucked)
            return None
        else:            
            next_point = self._bypass_way[self._bypass_way_sector + 1]
            line = Line(self._position,next_point)

            if line.length() == 0:
                self._bypass_way_sector += 1
                return self.getBypassWayDirection()
            elif line.length() <= self._step_length:
                return_value = {Brain.kPosition: next_point.getArr()}  

                self._bypass_way_sector += 1
                if self._bypass_way_sector == count_of_bypass_points - 1:
                    print "\n\nCheck point: ",self._previus_moving_state
                    self.setMovingState(self._previus_moving_state)

                return return_value
            else:
                direction = line.getAngle()
                x = math.cos(direction)
                y = math.sin(direction)
                return_value = {Brain.kDirection: [x,y]}
                return return_value

    def getGoHomeWayDirection(self):
        print "getGoHomeWayDirection"

        pos_arr = self._passed_way[self._passed_way_index]
        return_value = {Brain.kPosition: pos_arr}
        self._passed_way_index -= 1
        if self._passed_way_index == -1:
            self._passed_way_index = 0

            self.changePassedWayIfNeed()

            for observer in self.observers:
                if hasattr(observer,'food_brought'):
                    observer.food_brought()    

            self.setMovingState(MovingState.go_to_food)
        return return_value

    def getGoToFoodWayDirection(self):
        print "getGoToFoodWayDirection"
        pos_arr = self._passed_way[self._passed_way_index]
        count_of_position = len(self._passed_way)
        return_value = {Brain.kPosition: pos_arr}
        self._passed_way_index += 1
        if self._passed_way_index > count_of_position - 1:
            self._passed_way_index = count_of_position - 1                        
            self.setMovingState(MovingState.finding_new_piece_of_food)
        return return_value

    def getGoFindingNewPieceOfFoodDirection(self):
        print "getGoFindingNewPieceOfFoodDirection"
        return self.getSearchingDirection()

    def getGoNewfoodFounded(self):
        print "getGoNewfoodFounded"

        pos_arr = self._temp_passed_way_to_new_food[-1]
        return_value = {Brain.kPosition: pos_arr}

        count_of_steps = len(self._temp_passed_way_to_new_food)        
        print "test leave steps: ",count_of_steps

        if count_of_steps == 1:
            self.setMovingState(MovingState.go_to_home)
        
        self._temp_passed_way_to_new_food.pop()

        return return_value


    #################################################################
    ########   PRIVATE METHODS      #################################
    #################################################################

    def find_way_length(self,way):        
        way_len = 0
        for i in xrange(len(way)-1):
            cur_point = way[i]
            next_point = way[i+1]
            way_len += math.sqrt(pow(next_point[0]-cur_point[0],2)+pow(next_point[1]-cur_point[1],2))
        return way_len

    def find_bypass_way(self):
        print "find_bypass_way"
        selfLinFunc = LiniarFunction()
        selfLinFunc.setPointAndAngle(self._position,self._main_direction_angle)

        inter_dict = self._barrier.getSmallerShapeDevidedByFundtion(selfLinFunc)

        if inter_dict["is_intersect"]:
            if inter_dict.has_key("one_vertex"):
                p = inter_dict["one_vertex"]
                print "intersect in one vertex"
                p.log()
            if inter_dict.has_key("shape"):
                is_left_shape = inter_dict["is_left_shape"]
                print "\n\n-----------------\nIs left Shape: ",is_left_shape                      
                shape = inter_dict["shape"]               

                if not len(shape.points) < 3 :
                    p1 = shape.points[0]
                    p2 = shape.points[1]
                                            
                    line = Line(p1,p2)
                    h = line.height_to_point(self._position)

                    self._bypass_way = []                    
                    self._bypass_way.append(self._position)
                    self._bypass_way_sector = 0

                    last_point = shape.points[-1]

                    finish_point = last_point.move_in_direction_with_length(self._main_direction_angle,1)
                    shape.points.append(finish_point)

                    points_count = len(shape.points)
                    for i in range(points_count - 2):
                        p1 = shape.points[i]
                        p2 = shape.points[i+1]
                        p3 = shape.points[i+2]

                        line1 = Line(p1,p2)
                        line2 = Line(p2,p3)

                        hp1 = line1.point_of_height_vertex_to_right_direction(h,not is_left_shape)
                        hp2 = line2.point_of_height_vertex_to_right_direction(h,not is_left_shape)

                        lineFunc1 = LiniarFunction()
                        lineFunc1.setPointAndAngle(hp1,line1.getAngle())

                        lineFunc2 = LiniarFunction()
                        lineFunc2.setPointAndAngle(hp2,line2.getAngle())

                        is_inter, intersect_point = lineFunc1.intersect(lineFunc2)
                        if is_inter:
                            self._bypass_way.append(intersect_point)
                        else:                            
                            return False

                    return True
        else:
            print "Don't intersect"            

        return False

    