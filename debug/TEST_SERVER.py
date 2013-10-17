#!/usr/bin/env python
# Echo server program

import socket

HOST = 'localhost'                 # Symbolic name meaning all available interfaces
PORT = 65000              # Arbitrary non-privileged port

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

s.bind((HOST, PORT))
s.listen(1)
conn, addr = s.accept()
print 'Connected by', addr
while 1:
    data = conn.recv(1024)
    print data
    break
conn.close()
