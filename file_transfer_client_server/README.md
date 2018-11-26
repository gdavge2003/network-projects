CS372 Networks: a client-server file transfer application

References:
https://www.pythonforbeginners.com/cheatsheet/python-file-handling
https://docs.python.org/2/howto/sockets.html
https://docs.python.org/2/library/socket.html
https://stackoverflow.com/questions/6990474/how-can-i-override-the-keyboard-interrupt-python
https://stackoverflow.com/questions/166506/finding-local-ip-addresses-using-pythons-stdlib
https://docs.python.org/2/howto/sockets.html
https://docs.python.org/2/library/socket.html
https://stackoverflow.com/questions/6990474/how-can-i-override-the-keyboard-interrupt-python
https://stackoverflow.com/questions/166506/finding-local-ip-addresses-using-pythons-stdlib
Heavily reuses code from Project 1

Steps to run:
1) This is tested and ran successfully on 'flip2.engr.oregonstate.edu' and 'flip3.engr.oregonstate.edu'. Please run the programs there.
	Note: client and server files MUST be on different servers, or there will be a PORT CONFLICT.
2) Extract the files to servers:
	2a) Extract server files 'ftserver.c' and 'makefile' to a directory in 'flip3.engr.oregonstate.edu'.
	2b) Extract client file 'ftclient.py' to a directory in 'flip2.engr.oregonstate.edu'.
3) Open up two terminal windows, one for the client and another for the server.
4) For server:
	5a) Compile ftserver.c using 'make'
	5b) Start up the client: './client-server-file-transfer-app [server port]'
5) For client:
	4a) Give it executable permissions: 'chmod a+x ftclient.py'
	4b) Run the client using the appropriate arguments per project specs: 
		- 'python ftclient.py [flip server] [server port] [command: -l] [data port]'
		- 'python ftclient.py [flip server] [server port] [command: -g] [file name] [data port]'
7) Server will continuously run and accept connections until terminated via SIGINT. Client will terminate automatically after successsful task.

Example setup:
- Extract and compile server files on 'flip3.engr.oregonstate.edu'.
- Start the server app listening to port 31111 with './client-server-file-transfer-app 31111'

- Extract client file on 'flip2.engr.oregonstate.edu'.
- Give appropriate running permissions.
- Get directory listing of server via port 31112 by running 'ftclient.py flip3 31111 -l s2.txt 31112'
- Get file named s2.txt in current server directory via port 31112 by running 'ftclient.py flip3 31111 -g s2.txt 31112'