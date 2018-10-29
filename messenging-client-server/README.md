CS372 Networks: a client_name-server message application

References:
http://beej.us/guide/bgnet/
https://docs.python.org/2/howto/sockets.html
https://docs.python.org/2/library/socket.html
https://stackoverflow.com/questions/6990474/how-can-i-override-the-keyboard-interrupt-python
https://github.com/gdavge2003/operating-systems-projects/tree/master/encryptor-decryptor

Steps to run:
1) This is tested and ran successfully on 'flip3.engr.oregonstate.edu'. Please run the programs there.
2) Unzip chatclient.c, chatserver.py, and makefile to 'flip3.engr.oregonstate.edu'.
3) Open up two terminal windows, one for the client and another for the server.
4) For server:
	4a) Give it executable permissions: 'chmod a+x chatserver.py'
	4b) Start up the server: 'python chatserver.py [port #]'
5) For client:
	5a) Compile chatclient.c: 'make'
	5b) Start up the client: './client-server-message-app [flip3] [server port #]
6) Send the first message from client. Then alternate sending messages from server to client and so on. 
7) Input '\quit' on either client or server to break the connection. Server will continue to listen for new connections, while the client program will terminate.
