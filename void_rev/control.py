#/usr/bin/python
import os
import socket
import sys

# Create a UDS socket
sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)

# Connect the socket to the port where the server is listening
server_address = '/tmp/void-socket-default'
print >>sys.stderr, 'connecting to %s' % server_address
try:
        sock.connect(server_address)
except socket.error, msg:
        print >>sys.stderr, msg
        sys.exit(1)


try:
    message = "quit"
    print >> sys.stderr, 'sending "%s"' % message
    sock.sendall(message)

finally:
    sock.close()
                

