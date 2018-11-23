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

USER_HANDLE = "SERVER"

# primary method to handle session
def session(connection):

    # initial receiving and sending of user_handles
    client_name = connection.recv(1024) # receive client name
    print "Client name received: " + client_name
    connection.send(USER_HANDLE) # send server name
    print "Sent server name.\nBegin chatting session:\n"
    
    # ongoing client-server chat; only broken by /quit
    while 1:
        # open to receive from client
        receive = connection.recv(501)[0:-1]
        # terminate connection if user quits
        if receive == "\quit":
            print "Connection closed.\n"
            break

        print "{}> {}".format(client_name, receive)

        # sending message to client 
        send = ""
        while len(send) == 0 or len(send) > 500:
            send = raw_input("{}> ".format(USER_HANDLE))
            
        if send == "\quit":
            print "Connection closed.\n"
            connection.send("Server has quit the session.")
            break
        else:
            connection.send(send)


# method to handle ctrl+c gracefully. close the connection and exits the program
def signal_handler(signal, frame):
    print ' Closing program...\n'
    sys.exit(0)


# python script execution begins here (ie: main)
if __name__ == "__main__":
    # gracefully allows sigint exit
    signal.signal(signal.SIGINT, signal_handler)

    # setup desired port number to use
    if len(sys.argv) == 2:            
        port_number = sys.argv[1]
    else:
        print "Improper usage. Please follow format: 'python chatserver.py port_number' (ie: [1024, 65535]).\n" 
        exit(1)
    port_number = sys.argv[1]
    
    # setup socket type on port, then begin listening for requests
    server_socket = socket(AF_INET, SOCK_STREAM)    
    # '' - specifies that the socket is reachable by any address the machine happens to have     
    server_socket.bind(('', int(port_number)))
    server_socket.listen(1)
    
    # confirmation at this point
    print "Server is now listening for incoming connections.\n"

    # program will be kept alive indefinitely until SIGINT
    # if client cuts off connection, close connection but continue to listen
    while 1:
        # connection is the socket object, addr_tuple is a tuple of address and port of client
        connection, addr_tuple = server_socket.accept()
        print "Connected to: {}".format(addr_tuple)

        # manages the current client-server session until closed by sigint.
        session(connection)
        connection.close()

