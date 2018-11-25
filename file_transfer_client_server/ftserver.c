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
https://stackoverflow.com/questions/16010622/reasoning-behind-c-sockets-sockaddr-and-sockaddr-storage
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


//// This section contains struct addrinfo and methods to establish an initial connection
// method returns an addrinfo which has information for this server, which socket will use later
struct addrinfo* createAddress(char* port) {

    struct addrinfo hints;
    struct addrinfo *servinfo;
    int addr_status;

    // setup information on type of connection (TCP)
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // check to make sure it's successful
    if ((addr_status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "Error setting up server info.\n"
                        "Server setup error: %s\n", gai_strerror(addr_status));
        exit(1);
    }

    return servinfo;
}

// create socket for given addr info
int createSocket(struct addrinfo* servinfo) {

    int sockfd;

    // setup socket descriptor with server info and error check
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        fprintf(stderr, "Error setting up socket descriptor.\n");
        exit(1);
    }

    return sockfd;
}

// bind functioning socket to given port on server
void bindSocket(int sockfd, struct addrinfo* servinfo) {

    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(sockfd);
        fprintf(stderr, "Error binding socket to given port on this server.\n");
        exit(1);
    }
}

// set established socket on server to begin listening for connections
void listenSocket(int sockfd) {

    if (listen(sockfd, 5) == -1) {
        close(sockfd);
        fprintf(stderr, "Error established socket unable to listen.\n");
        exit(1);
    }
}


//// This section contains struct addrinfo and methods for ongoing file transfer sessions

// establish connection with client to send data
void connectSocket(int sockfd, struct addrinfo* servinfo) {

    int connect_status;

    // connect socket and error check
    if ((connect_status = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen)) == -1) {
        fprintf(stderr, "Error connecting socket to server port.\n");
        exit(1);
    }
}


// handles client-server interactions using helper methods
void interactWithClient(int client_fd) {

    // setup strings for storage
    char data_port[50];

    memset(data_port, 0, sizeof(data_port));
    recv(client_fd, data_port, sizeof(data_port)-1, 0);

    printf("%s\n\n", data_port);























}

// takes in client socket, input user handle and empty serverName array (which gets what server sends)
void session(int sockfd) {

    printf("Server established. Now accepting incoming connections...\n");

    // setup variables to store incoming connection data
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    int incoming_fd;


    while (1) {
        addr_size = sizeof(client_addr);

        if ((incoming_fd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_size)) == -1) {
            fprintf(stderr, "Error establishing connection with incoming request.\n"
                            "Resuming listening...\n");
            continue;
        }

        // connection successful - use incoming_fd to interact with client
        interactWithClient(incoming_fd);

        // close connection with client once interaction is finished; loop to accept more incoming
        close(incoming_fd);
        printf("Connection with client closed.\n");
    }
}


//// Main
int main(int argc, char *argv[]) {

    // check for proper arguments. must be [port_num]
    if (argc != 2){
        fprintf(stderr, "Improper usage. Please do: 'file_transfer_client_server <PORT_NUM>'\n");
        exit(1);
    }

    // setup struct addrinfo with server info given user-input port
    struct addrinfo* servinfo = createAddress(argv[1]);

    // create, establish, and set socket on port number to be ready to listen for connections
    int sockfd = createSocket(servinfo);
    bindSocket(sockfd, servinfo);
    listenSocket(sockfd);

    // connected - session begins until otherwise closed
    session(sockfd);

    // clean up
    freeaddrinfo(servinfo);
}
