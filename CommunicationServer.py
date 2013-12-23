#!/usr/bin/env python

import socket
import ConfigParser

class CommunicationServer:
    def __init__(self):
        self._config=ConfigParser.RawConfigParser()
        self._config.read('conf/config.conf')
        self._server_host = self._config.get("Ant",'server_host')
        self._server_port = 55000

        pass

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
            #print "socket accept"
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
                            #print "server()::request goted."
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
                            print "way length %s"%str(way_length)
                            
                            if way_length < self._success_way_distance: # if new way is better
                                print "server()::New way is better. Update success way."
                                print "server()::",type(j["OBJECT"]["WAY_COORDS"])
                                #self._new_way = j["OBJECT"]["WAY_COORDS"]
                                #self.go_home("from server")
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

