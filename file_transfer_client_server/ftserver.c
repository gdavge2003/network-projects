/**'''
Linge Ge
CS372 Fall 2018 - Project 2: server side of file transfer application
'''
'''
ftserver.c serves as the server in the client-server file transfer application.
'''
'''
References:
http://beej.us/guide/bgnet/
https://stackoverflow.com/questions/16010622/reasoning-behind-c-sockets-sockaddr-and-sockaddr-storage
https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
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
#include <unistd.h>
#include <dirent.h>
#include <limits.h>

#define MAX_MSG_LENGTH	500


//// This section contains struct addrinfo and methods to establish an initial connection
// method returns an addrinfo which has information for this server, which socket will use later
struct addrinfo* createAddress(char* port) {
    struct addrinfo hints;
    struct addrinfo *servinfo;
    int addr_status;

    // setup information on type of connection (TCP)
    memset(&hints, '\0', sizeof(hints));
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


//// This section contains struct addrinfo and methods for 2nd data socket setup and connection
// struct to hold information to setup data port connection
struct addrinfo* createDataAddress(char* client_address, char* port) {
    struct addrinfo hints;
    struct addrinfo *clientinfo;
    int addr_status;

    // setup information on type of connection (TCP)
    memset(&hints, '\0', sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // check to make sure it's successful
    if ((addr_status = getaddrinfo(client_address, port, &hints, &clientinfo)) != 0) {
        fprintf(stderr, "Error setting up data info.\n"
                        "Data port setup error: %s\n", gai_strerror(addr_status));
        exit(1);
    }

    return clientinfo;
}

// establish connection with client to send data
int connectSocket(int sockfd, struct addrinfo* servinfo) {
    int connect_status;

    // connect socket and error check
    if ((connect_status = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen)) == -1) {
        fprintf(stderr, "Error connecting socket to data port for data connection.\n");
        return -1;
    }

    return 1;
}


//// This section contains methods related to processing requset and sending correct data back
// counts the number of files currently in the directory
int getDirectoryCount() {
    int count = 0;
    struct dirent* de;
    DIR *dr = opendir(".");

    if (dr == NULL) {
        fprintf(stderr, "Error opening current directory.\n");
        return -1;
    }

    while ((de = readdir(dr)) != NULL)
        count++;
    closedir(dr);

    return count;
}

// create string array to hold file names
char** createStringArray(int size) {
    char** array = malloc(size * sizeof(char*));

    int i;
    for (i = 0; i < size; i++){
        array[i] = malloc(256 * sizeof(char));
        memset(array[i], '\0', 256);
    }

    return array;
}

// clear memory of string array used to hold file names
void deleteStringArray(char** array, int size) {
    int i;
    for (i = 0; i < size; i++) {
        free(array[i]);
    }

    free(array);
}

// takes in empty array, and fills file names of current directory with it
int getDirectoryContents(char** array) {
    struct dirent* de;
    DIR *dr = opendir(".");
    int i = 0;

    if (dr == NULL) {
        fprintf(stderr, "Error opening current directory.\n");
        return -1;
    }

    while ((de = readdir(dr)) != NULL) {
        if (de->d_type == DT_REG) {
            strcpy(array[i], de->d_name);
            i++;
        }
    }

    closedir(dr);

    return 1;
}

// returns boolean if requested file name matches one of directory contents
int findFile(char** file_names, int count, char* file_name) {
    int i;
    for (i = 0; i < count; i++) {
        if (strcmp(file_names[i], file_name) == 0) {
            return 1;
        }
    }

    return 0;
}

// processes sent file
void sendFile(int data_socket, char* file_name) {
    char* not_found_message = "File not found.";
    char* error_message = "Error occurred. Failed file retrieval.";
    char* found_message = "File found. Server sending over file.";

    // get name of current working directory
    char cur_dir[PATH_MAX];
    if (getcwd(cur_dir, sizeof(cur_dir)) == NULL) {
        fprintf(stderr, "Error attempting to get current directory name.\n");
        send(data_socket, error_message, strlen(not_found_message), 0);
        return;
    }

    // get file count and if it's empty, then just return not found
    int files_num = getDirectoryCount();
    if (files_num == -1) {
        fprintf(stderr, "Could not get directory count for %s.\n", cur_dir);
        send(data_socket, error_message, strlen(not_found_message), 0);
        return;
    }
    if (files_num == 0) {
        send(data_socket, not_found_message, strlen(not_found_message), 0);
        return;
    }

    // do a search and if not found, then return not found
    char** file_names;
    file_names = createStringArray(files_num);
    int res = getDirectoryContents(file_names);
    if (res == -1) {
        fprintf(stderr, "Error retrieving file names for directory: %s.\n", cur_dir);
        send(data_socket, error_message, strlen(not_found_message), 0);
        deleteStringArray(file_names, files_num);
        return;
    }

    int isFound = findFile(file_names, files_num, file_name);
    if (!isFound) {
        send(data_socket, not_found_message, strlen(not_found_message), 0);
        deleteStringArray(file_names, files_num);
        return;
    }

    // found file - send over contents
    send(data_socket, found_message, strlen(found_message), 0);





    // cleanup
    deleteStringArray(file_names, files_num);
}

// processes directory information and sends to client
void sendDirectoryInfo(int data_socket) {
    // get name of current working directory
    char cur_dir[PATH_MAX];
    if (getcwd(cur_dir, sizeof(cur_dir)) == NULL) {
        fprintf(stderr, "Error attempting to get current directory name.\n");
        return;
    }

    // get the number of files in current directory
    int files_num = getDirectoryCount();
    if (files_num == -1) {
        fprintf(stderr, "Could not get directory count for %s.\n", cur_dir);
        return;
    }

    // get array of file names in current directory
    char** file_names;
    char message[500];
    memset(message, '\0', sizeof(message));
    strcpy(message, "Current directory is: ");
    strcat(message, cur_dir);
    strcat(message, ". The files in the directory are:");
    send(data_socket, message, strlen(message), 0);

    // if there are actually files in the directory, send them along. Otherwise just mention empty dir
    if (files_num > 0) {
        file_names = createStringArray(files_num);
        int res = getDirectoryContents(file_names);
        if (res == -1) {
            fprintf(stderr, "Error retrieving file names for directory: %s.\n", cur_dir);
        }

        int i;
        for (i = 0; i < files_num; i++) {
            // strcat(file_names[i], "\n");
            send(data_socket, file_names[i], 256, 0);
        }
    }
    else {
        char* empty_dir = "[empty]";
        send(data_socket, empty_dir, strlen(empty_dir), 0);
    }

    // send an expected keyword to indicate end of transmission
    char* end = "end transmission";
    send(data_socket, end, strlen(end), 0);

    // cleanup
    if (files_num > 0) {
        deleteStringArray(file_names, files_num);
    }
}

// handles client-server interactions using helper methods
void interactWithClient(int client_fd) {
    // validator messages
    char* valid = "1";
    char* invalid ="-1";
    char* bad_command = "Command not valid. Only -g, -l allowed\n.";

    // receive and validate command. If invalid command, return prematurely
    char command[5];
    memset(command, '\0', sizeof(command));
    recv(client_fd, command, sizeof(command)-1, 0);

    if (strcmp(command, "-g") != 0 && strcmp(command, "-l") != 0) {
        send(client_fd, bad_command, strlen(bad_command), 0);
        printf("Closing connection to client.\n");
        return;
    }
    else {
        send(client_fd, valid, strlen(valid), 0);
    }

    // receive data port. no need to validate as client already validates correct data port
    char data_port[10];
    memset(data_port, '\0', sizeof(data_port));
    recv(client_fd, data_port, sizeof(data_port)-1, 0);
    send(client_fd, valid, strlen(valid), 0);

    // receive client address, which is needed to establish data connection later
    char client_address[100];
    memset(client_address, '\0', sizeof(client_address));
    recv(client_fd, client_address, sizeof(client_address)-1, 0);
    send(client_fd, valid, strlen(valid), 0);

    // receive file name if -g
    char file_name[256];
    memset(file_name, '\0', sizeof(file_name));
    if (strcmp(command, "-g") == 0) {
        recv(client_fd, file_name, sizeof(file_name)-1, 0);
        send(client_fd, valid, strlen(valid), 0);
    }
    else {
        file_name[0] = '-'; // simple checker for debugging purposes
    }

    printf("Client Data: %s %s %s %s\n", command, data_port, client_address, file_name);

    // at this point: all data needed to process has passed in. Setup data connection
    sleep(3); // waits for client to successfully setup the listening socket before initiating
    struct addrinfo* clientinfo = createDataAddress(client_address, data_port);
    int data_socket = createSocket(clientinfo);
    if (connectSocket(data_socket, clientinfo) == -1) {
        close(data_socket);
        freeaddrinfo(clientinfo);
        return;
    }

    // process particular given command
    if (strcmp(command, "-l") == 0) {
        printf("Client requests current directory information. Processing...\n");
        sendDirectoryInfo(data_socket);
    }
    else if (strcmp(command, "-g") == 0) {
        printf("Client requests file '%s'. Processing...\n", file_name);
        sendFile(data_socket, file_name);
    }

    printf("Process completed.\n");
    close(data_socket);
    freeaddrinfo(clientinfo);
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
        printf("Client requested connection. Connection established successfully.\n");
        interactWithClient(incoming_fd);

        // close connection with client once interaction is finished; loop to accept more incoming
        close(incoming_fd);
        printf("Connection with client closed.\n");
        printf("Continuing to accept incoming connections...\n");
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
