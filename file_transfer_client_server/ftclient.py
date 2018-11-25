#!/bin/python

'''
Linge Ge
CS372 Fall 2018 - Project 2: client-server file transfer application
'''

'''
Written to be compatible with Python 2.7
ftclient.py serves as a client for the client-server file transfer application.
It establishes connection to a running server app on another host, sends a command,
and receives the requested information.
'''

'''
References:
https://docs.python.org/2/howto/sockets.html
https://docs.python.org/2/library/socket.html
https://stackoverflow.com/questions/6990474/how-can-i-override-the-keyboard-interrupt-python
Heavily reuses code from Project 1
'''

import sys
from socket import *
import signal


# global variables
FLIP_SERVERS = ["flip1", "flip2", "flip3"]
HOST_ADDRESS = ".engr.oregonstate.edu"


# setup socket and connect to server
def connect_to_server(server, port):
    c_socket = socket(AF_INET, SOCK_STREAM)
    c_socket.connect((server, port))

    return c_socket

# primary method to handle session
def session(client_socket, command, data_port, filename):

    client_socket.send(data_port)

    client_socket.close()


# method to handle ctrl+c gracefully. close the connection and exits the program
def signal_handler(signal, frame):
    print ' Closing program...\n'
    sys.exit(0)


# python script execution begins here (ie: main)
if __name__ == "__main__":

    # gracefully allows sigint exit
    signal.signal(signal.SIGINT, signal_handler)

    # check correct arguments
    # program <SERVER_HOST>, <SERVER_PORT>, <COMMAND>, <FILENAME>, <DATA_PORT>
    if len(sys.argv) < 5 or len(sys.argv) > 6:
        print "Incorrect arguments. Please use either of the following:\n" \
            "1) python ftclient.py [flip server] [server port] [command: -l] [data port]\n" \
            "2) python ftclient.py [flip server] [server port] [command: -g] [file name] [data port]\n"
        exit(1)

    # validate correct server host
    flip_server = argv[1]
    if flip_server not in FLIP_SERVERS:
        print "Invalid server, please only just use flip hostname.\n"
        exit(1)
    flip_server = flip_server + HOST_ADDRESS

    # validate server port type and range
    server_port = argv[2]
    if not (1024 <= int(server_port) <= 65535):
        print "Invalid server port value. Valid range: [1024, 65535].\n"
        exit(1)

    # validate command
    command = argv[3]
    if command != "-g" and command != "-l":
        print "Invalid command. Please use '-l' or -'g'.\n"
        exit(1)

    # validate filename and data port
    filename, data_port = None, None
    if command == "-l" and len(sys.argv) == 5:
        data_port = argv[4]
    elif command == "-g" and len(sys.argv) == 6:
        filename = argv[4]
        data_port = argv[5]
    else:
        print "Invalid command/argument combination.\n" \
              "Please use '-l [data port]' OR '-g [file name] [data port]'.\n"
        exit(1)

    # validate data port
    if not (1024 <= int(data_port) <= 65535) or int(data_port) == int(server_port):
        print "Invalid data port value, or data and server ports are assigned to the same. " \
                "Valid range: [1024, 65535].\n"
        exit(1)

    # establish connection to server
    client_socket = connect_to_server(flip_server, server_port)

    # interaction handling once connection is established
    session(client_socket, command, data_port, filename)


