/**'''
Linge Ge
CS372 Fall 2018 - Project 1: client_name-server message application
'''

'''
chatsclient.c serves as the client in the client_name-server message application.
'''

'''
References:
http://beej.us/guide/bgnet/
this is my CS344 project on sockets: https://github.com/gdavge2003/operating-systems-projects/tree/master/encryptor-decryptor 
'''
*/


#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <zconf.h>
#include <fcntl.h>

#define MAX_MSG_LENGTH	500

// method returns an addrinfo which has information on the server to connect to
struct addrinfo* createAddress(char* address, char* port) {

	struct addrinfo hints;
    struct addrinfo *servinfo;
    int addr_status;
	
	// setup information on type of connection (TCP)
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	// check to make sure it's successful
	if((addr_status = getaddrinfo(address, port, &hints, &servinfo)) != 0) {
    	fprintf(stderr, "Error setting up server info.\ngetaddrinfo error: %s\n", gai_strerror(addr_status));
    	exit(1);
	}
	
	return servinfo;
}

// method sets up the socket on the client side AFTER server information is setup in struct addrinfo
int setupSocket(struct addrinfo* servinfo) {
	
	int sockfd;                                               
	int connect_status;

	// setup socket descriptor with server info and error check
	if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
		fprintf(stderr, "Error setting up socket descriptor.\n");
		exit(1);
	}

	// connect socket and error check
	if ((connect_status = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen)) == -1) {
		fprintf(stderr, "Error connecting socket to server socket.\n");
		exit(1);
	}

	return sockfd;
}

// takes in client socket, input user handle and empty serverName array (which gets what server sends)
void session(int sockfd, char * userHandle, char * serverName) {

	// initial communication with server to share handles
	// note: chatserver.py should be programmed in acknowledging client sends first, then sends it's own handle!! 
	int sendingcode = send(sockfd, userHandle, strlen(userHandle), 0);	
	int receivingcode = recv(sockfd, serverName, 10, 0);

	// make sure initials are successful
	if (sendingcode == -1) {
			fprintf(stderr, "Error in sending handle to server.\n");
			exit(1);
		}
	if (receivingcode == -1) {
			fprintf(stderr, "Error in receieving handle from server.\n");
			exit(1);
		}

	printf("Server name received: %s.\nBegin chatting session:\n\n", serverName);

	// setup buffers for both messages - up to MAX_MSG_LENGTH chars/msg allowed
	char sendMsg[MAX_MSG_LENGTH];
    char receiveMsg[MAX_MSG_LENGTH];
    memset(sendMsg, 0 ,sizeof(sendMsg));
	memset(receiveMsg, 0, sizeof(receiveMsg));
    
    // error tracking for sent and received messages
	int status;
	int byte = 0;

    // 1st user message sent
	fgets(sendMsg, MAX_MSG_LENGTH, stdin);
    
	while(1) {
		// client sends first message
		printf("%s> ", userHandle);
		fgets(sendMsg, MAX_MSG_LENGTH, stdin);
		
		// if user types in quit, then break out of this loop and leads to closed connection
		if (strcmp(sendMsg, "\\quit\n") == 0) {
			byte = send(sockfd, sendMsg, strlen(sendMsg), 0);
			if (byte == -1) {
				fprintf(stderr, "Error in sending message to server.\n");
				exit(1);
			}
			
			break;
		}
		
		// send and check byte count to make sure bytes sent is correct 
		byte = send(sockfd, sendMsg, strlen(sendMsg), 0);
		if (byte == -1) {
			fprintf(stderr, "Error in sending message to server.\n");
			exit(1);
		}
		
		// receive and check for receiving errors
		status = recv(sockfd, receiveMsg, 500, 0);
		if (status == -1) {
			fprintf(stderr, "Error in receieving message from server.\n");
			exit(1);
		}
		else if (status == 0) {
			printf("Program ended by server.\n");
			break;
		}
		else {
			printf("%s> %s\n", serverName, receiveMsg);
		}
		
		// clear char buffer for next set of messages
		memset(sendMsg, 0, sizeof(sendMsg));
		memset(receiveMsg, 0, sizeof(receiveMsg));
	}
	
	// close connection
	close(sockfd);
	printf("Connection closed.\n");
}


// Main
int main(int argc, char *argv[]) {

	// check for proper arguments. must be [hostname] [port_num]
	if(argc != 3){
		fprintf(stderr, "Improper usage. Please follow format: './this_app hostname server_port_number' (ie: [1024, 65535]).\n");
		exit(1);
	}
	
	char userHandle[10];
    char serverName[10];

    // setup client user handle
    printf("Enter a userHandle that is 10 characters or less. ");
	scanf("%s", userHandle);	
    
    // setup struct addrinfo with server info
	struct addrinfo* servinfo = createAddress(argv[1], argv[2]);
	
	// setup client-side socket and connect to server
	int sockfd = setupSocket(servinfo);
	   
	// connected - session begins until otherwise closed
	session(sockfd, userHandle, serverName);
	
	// clean up
	freeaddrinfo(servinfo);
}

