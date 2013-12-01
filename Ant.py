#!/usr/bin/env python

'''
    Study project for ant algorithm simulation using multiagent system.

    @Author Barchynskyi Maksym gr 406
'''

import socket
import ConfigParser
import time
import sys
import json
import warnings
import math
import random
from Brain import *

sys.path.append('templates')
from templates import *
import threading

class Server(threading.Thread):
    def __init__(self,host):
        print "And server initilized..."
        super(Server,self).__init__()
        self._host = host
        self._port = self.get_unused_port()

        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def run(self):
        print "Ant server running..."
        self.socket.bind((self._host,self._port))
        self.socket.listen(1)
        conn, addr = self.socket.accept()
        while True:
            data = conn.recv(1024)
            print data
            
        conn.close()

    def get_unused_port(self):
        print "AntServer::Loking for free port..."
        temp_s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        temp_s.bind(('localhost', 0))
        addr, port = temp_s.getsockname()
        temp_s.close()
        print "Port found:"+str(port)
        return int(port)

    def get_socket(self):
        return str(self._host)+':'+str(self._port)


class Ant(threading.Thread):
    def __init__(self,exteranl_id):
        print "Ant::Init()"
        super(Ant,self).__init__()
        self._config=ConfigParser.RawConfigParser()
        self._config.read('conf/config.conf')
        self._client_host=self._config.get("Ant","host")
        self._port=self._config.getint("Ant","port")
        self._server_host = self._config.get("Ant",'server_host')
        self._server_port = 55000
        self._sleep_time = self._config.getfloat("Ant","sleep")
        
        self.client_socket=socket.socket(socket.AF_INET,socket.SOCK_STREAM) # socket connects to anthell server

        self._external_id = exteranl_id
        self._id=int(time.time())

        self._brain = Brain()
        
        #self.ping_server(self._server_host,self._port) # stop main thread if host unreachable

        self.server_thread = threading.Thread(target=self.server)
        self.client_thread = threading.Thread(target=self.live)


        ###################################
        '''self method debug zone'''
        #self.register()

        ###################################

    def server(self):
        print 'Server is working...'
        server_host = self._server_host
        self._server_port = self.get_unused_port()
        print self._server_port

        try:
            #print "server::Try section"
            ssocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            ssocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            print "server::socket created"

            ssocket.bind((server_host,self._server_port))
            #print "Socket bind"
            ssocket.listen(1000)
            #print "socker listen"
            #conn, addr = ssocket.accept()
            print "socket accept"
            while True:
                ssocket.setblocking(0)
                timeout = 1
                total_data = []
                start_time = time.time()
                
                while True:
                    if total_data and time.time() > timeout:
                        break
                    elif time.time() - start_time > timeout*2:
                        break
                try:
                    conn, addr = ssocket.accept()
                    #print "."
                    data = conn.recv(BUFFSIZE)
                    #print "server()::data received:",data
                    if data:
                        total_data.append(data)
                        start_time = time.time()
                        print "server()::Data not empty. Continue...",data
                        if self.is_client_request(data) : # if current message is request - send response
                            print "server()::request goted."
                            print "Food found=",self._is_food_found
                            if self._is_food_found: # response if success way exists
                                print 'server()::success way exists. Perform response generating...'
                                response = self.create_get_found_way_response_query()
                                conn.sendall(response)
                                conn.close()
                                print "server()::Response sended to %s"%str(addr),response
                            else:
                                print "server()::No success way found. Response NOT sended."
                                #conn.send("Null data")
                                conn.close()

                        else: # process response
                            print "server()::Response received.",data
                            j = json.dumps(data)
                            print "Json",str(j)
                            way_length = j["OBJECT"]["WAY_LENGTH"]
                            #print "////////////",type(way_length),way_length
                            print "way length %s"%str(way_length)
                            
                            if way_length < self._success_way_distance: # if new way is better
                                print "server()::New way is better. Update success way."
                                print "server()::",type(j["OBJECT"]["WAY_COORDS"])
                                self._new_way = j["OBJECT"]["WAY_COORDS"]
                                self.go_home("from server")
                                #self._success_way_distance = j["OBJECT"]["WAY_COORDS"] # update success way
                                conn.close()
                    else:
                        print "server::connection lost"
                        ssocket.close()
                        sys.exit()
                        #print "No logical data accepted."
                        pass

                except:
                    pass
            if total_data:
                total_data = b''.join(total_data)
                print "total data reveived: %s"%total_data

        except Exception,e:
            print "server()::Exception:"+str(e)
            print "server()::End flow."
            sys.exit()
            
        conn.close()
    def is_client_request(self,msg):
        '''
        Check if msg is request.
        '''
        #print "is_client_request(self,msg)::message:",msg
        try:
            j = json.loads(msg)
            obj = j["OBJECT"]
            #print "is_client_request()::obj len=",len(obj)
            print "is_client_request::",len(obj.items())
            return True if not len(obj.items()) else False

        except Exception,e:
            print "is_client_request(self,msg):",str(e)

    def get_unused_port(self):
        #print "AntServer::Loking for free port..."
        temp_s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        temp_s.bind(('127.0.0.1', 0))
        addr, port = temp_s.getsockname()
        temp_s.close()
        #print "Port found:"+str(port)
        return int(port)

    def ping_server(self,host,port):
        '''
        Break main thread if main server is down.
        '''
        try:
            #print "Ant::Loking for server..."
            temp_s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            temp_s.bind((host, int(port)))
            addr, port = temp_s.getsockname()
            temp_s.close()
            #print "Ant::Main server alive."
        except Exception,e:
            print "Main server is unreachable. Exit main thread."
            sys.exit()

    def connect(self,host,port):
        #print "Ant connecting to main server..."
        try:
            self.client_socket.connect((host,int(port))) # connect to host
            #print "Connected."
        except Exception,e:
            #print "Connection failed:",str(e)
            sys.exit()

    def disconnect(self):
        self.client_socket.close()

    def register(self):
        '''
        Send client registration query to Server.
        '''
        reg_query = self.create_reg_query(self._id)
        #print "register::Registration query:",reg_query
        self.send(reg_query)
        
    def send(self,query):
        '''
        Send data to server
        '''
        #print "send::Query to send:%s"%query

        try:
            self.client_socket.sendall(query.encode('utf-8'))
            #print "send::Data sended."
            response = self.client_socket.recv(1024)
            #print "send::Response goted:"#, response
            
            if response:
                self.process_response(response)
            else:
                #print "send::No response goted."
                pass
        except Exception,e:
            print "send::"+str(e)

    def process_response(self,response):
        '''
        Process all responses from server.
        '''
        j = json.loads(response)
        
        api_key = j["API_KEY"]

        if api_key == "registration":
            #print "Registration section found."
 
            x,y = j["OBJECT"]["COORD_ANT"]
            self._brain.setPosition(x,y)
            #print "Coordinates",self._pos_x, self._pos_y

        if api_key == "is_ant_can_move":
            #print "process_response::Is ant can move processing."
            if len(j["OBJECT"].items()) > 0 :
                #print "**process_response::Ant can move. Set direction to 45 degree."
                x,y = j["OBJECT"]["COORD_ANT"]
                self._brain.setPosition(x,y)
                #print "process_response::New coordinates goted:",x,y                
            else:
                self._brain.antStucked()
                #print "**process_response::Can't move with last direction... Choose another direction."
                direction = random.uniform(-1,1)

                self._direction_angel -= random.randint(60,100)*[-1,direction][bool(direction)]

                #print "self.direction_angel:",self._direction_angel
                #print "process_response::self.direction=%s"%str(abs(self._direction_angel))

        if api_key == "ERROR":
            pass
            #print "process_response::Error section found."
        
        if api_key == "nearest_objects":
            #print "Nearest object response goted.\n"
            if len(j["OBJECT"].items()) > 0:
                barriers = j["OBJECT"]["BARRIERS"]
                objects = j["OBJECT"]
                foods = j["OBJECT"]["FOODS"]
                ants = j["OBJECT"]["ANTS"]
                if foods:
                    self._success_way_distance = self.find_way_length( self._passed_way )

                    self._is_moving_back = True
                    print "process_responce::ff"
                    self._is_food_found = True
                    self.send(self.create_ant_got_food_query())
                    print "process_response::got_food request sended"
                    #raw_input("Food found.")

                if len(ants) > 0: # if there is ants
                    print "process_response()::ants found"
                    for ant in ants: # generate success way request
                        sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

                        ant_socket = ant["SOCKET"]
                        print "ant socket goted",ant_socket
                        host,port = ant_socket.split(":")
                        try:
                            sock.connect( (host,int(port)) )
                            sock.sendall( self.create_get_found_way_request_query() ) # send request about passed way
                            print "Request sended to port",port
                            data = sock.recv(1024**2)
                            if data:
                                print "WOWW"
                                print "Ants intersects, data transmition completed."
                                self.process_ant_response(data)
                                
                            sock.close()
                        except Exception,e:
                            print "process_response()::sock.sendall(*) error occured.",str(e),"port:",port
                        print "ip:%s,port:%s"%(host,port)
            else:
                print "No objects found. Go on..."
        if api_key == "get_found_way":
            print "WOW"

    def create_reg_query(self,ant_id):
        '''
        Create registration query.
        '''
        query={"API_KEY":"registration","OBJECT":{"SOCKET":self.get_socket()}}
        return json.dumps(query)

    def process_ant_response(self,data):
        try:
            print "server()::process_ant_response data received.",data
            j = json.loads(data)
            print "Json",str(j)
            try:
                way_length = j["OBJECT"]["WAY_LENGTH"]
            except Exception,e:
                print "wle"
            print "////////////",type(way_length),way_length
            print "way length %s"%str(way_length),type(way_length)
            
            if float(way_length) < self._success_way_distance: # if new way is better
                print "server()::New way is better. Update success way."
                print "server()::",type(j["OBJECT"]["WAY_COORDS"])
                #self._new_way = j["OBJECT"]["WAY_COORDS"]
                #self.go_home("from server")
                #conn.close()
                print "gohomefromserver"
            else:
                print 'new way length:%s. Own way length:%s'%(way_length,self._success_way_distance)
        except Exception,e:
            print "process_ant_response::%s"%(str(e))




    def get_socket(self):
        return str(self._server_host)+':'+str(self._server_port)

    def create_is_ant_can_move_query(self,vector_x,vector_y):
        '''
        Generate json is_ant_can_move query.
        '''
        query={"API_KEY":"is_ant_can_move","OBJECT":{"VECTOR":[vector_x,vector_y]} }
        return json.dumps(query)

    def create_ant_got_food_query(self):
        query = {"API_KEY":"got_food", "OBJECT":{}}
        print "create_ant_got_food_query"
        return json.dumps(query)

    def create_ant_put_food_query(self):
        query={"API_KEY":"put_food","OBJECT":{}}
        print "create_ant_put_food_query"
        return json.dumps(query)

    def create_is_ant_can_move_query_using_coords(self,coord_x,coord_y):
        '''
        Generate json is_ant_can_move query.
        '''
        query={"API_KEY":"is_ant_can_move","OBJECT":{"NEXT_COORD":[coord_x,coord_y]} }
        return json.dumps(query)

    def create_get_nearest_object_query(self):
        '''
        Create query for retrieving list of nearest objects.
        '''
        #query={"API_KEY":"nearest_objects","OBJECT":{"COORD":[self._pos_x,self._pos_y],"VISION_RANGE":0.05}}
        #print  "Get nearest object request sended."
        query={"API_KEY":"nearest_objects","OBJECT":{}}
        return json.dumps(query)

    def create_get_found_way_request_query(self):
        query={"API_KEY":"get_found_way","OBJECT":{}}
        return json.dumps(query)

    def create_get_found_way_response_query(self):
        query = {"API_KEY":"get_found_way","OBJECT":{ "WAY_LENGTH":self.find_way_length(self._passed_way), "WAY_COORDS": self._passed_way }}
        #print query
        return json.dumps(query)

    def get_possible_direction(self):
        '''
        Retrive data from server about possible way
        posible_way - array of possible moving directions
        '''
        rad = random.randint(0,360)
        #print "get_posible_direction::self._direction_angel=%s"%str((self._direction_angel))

        vector_x = math.cos(K*(self._brain.main_direction_angle)/4)
        vector_y = math.sin(K*(self._brain.main_direction_angle)/4)
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
        self.connect(self._client_host,self._port)
        self.register()
        print "Ant alive"
        while True:
            if not self._is_moving_back and not self._is_food_found:
                self.move()
                self.send( self.create_get_nearest_object_query() )
                print "live::I'm alive!"
                time.sleep(self._sleep_time)
            elif self._is_moving_back:
                self.go_home()
            elif self._is_food_found and not self._is_moving_back:
                self.go_to_resource()

    def go_home(self,s=""):
        '''
        Return home using coords from self.passed_way.
        '''
        print s
        print "go_home()"
        print "success way=%s steps"%str(len(self._passed_way))
        #while 1:
        for i in range( len(self._passed_way)-1, 0, -1 ):
        #for coords in self._passed_way.reverse():
            #print passed_way[i]
            x,y = self._passed_way[i]
            #print "Current coords:(%s,%s)"%(x,y)
            self.send( self.create_is_ant_can_move_query_using_coords(x,y) )
            self.send( self.create_get_nearest_object_query() )

            time.sleep(self._sleep_time)

        self._is_moving_back = False
        #print "End go_home()"
        self._passed_way = self._new_way if len(self._new_way) else self._passed_way # replace passed way with new way if the last is
        self.send(self.create_ant_put_food_query())
        #raw_input("End go home?")
        #self.go_to_resource()
        print "End go home."

    def go_to_resource(self):
        '''
        Go to resource using passed way.
        '''
        print "go_to_resource()"
        print "success way=%s steps"%str(len(self._passed_way))
        for i in range(0,len(self._passed_way)-1):
        #for coords in self._passed_way.reverse():
            #print passed_way[i]
            x,y = self._passed_way[i]
            #print "Current coords:(%s,%s)"%(x,y)
            self.send( self.create_is_ant_can_move_query_using_coords(x,y) )
            self.send( self.create_get_nearest_object_query() )

            time.sleep(self._sleep_time)

        self._is_moving_back = True
        #self._is_moving_back = False
        #self.go_home()
        #print "End go_to_resource()"
        #raw_input("End go to resource?")
        #pass

    def find_way_length(self,way):
        '''
        Find way length.
        '''
        way_len = 0
        for i in xrange(len(way)-1):
            cur_point = way[i]
            next_point = way[i+1]
            way_len += math.sqrt(pow(next_point[0]-cur_point[0],2)+pow(next_point[1]-cur_point[1],2))
        return way_len

    def run(self):
        self.server_thread.start()
        self.client_thread.start()

        self.client_thread.join()
        self.server_thread.join()

# /////////////////////// DEBUG ZONE ///////////////////////////
if __name__=='__main__':
    test_ant = Ant(0)
    #test_ant.live()
    test_ant.start()
    test_ant.join()
