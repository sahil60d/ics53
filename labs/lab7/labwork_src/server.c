#ifndef SAMPLE_H
#define SAMPLE_H

#include "sample.h"

#endif

static int the_number = 0;

void run_server(int server_port){
    int sockfd, clientfd, len;
    struct sockaddr_in servaddr, cli;

    char buffer[BUFFER_SIZE];

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("Socket successfully created\n");

    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(server_port);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("Socket successfully binded\n");
	while (1) {
    // Now server is ready to listen and verification
    if ((listen(sockfd, 1)) != 0) {
        printf("Listen failed\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("Server listening on port: %d.. Waiting for connection\n", server_port);

    len = sizeof(cli);
    // Wait and Accept the connection from client
    clientfd = accept(sockfd, (SA*)&cli, &len);
    if (clientfd < 0) {
        printf("server acccept failed\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("Client connetion accepted\n");

    bzero(buffer, BUFFER_SIZE);
	while (strcmp(buffer, "exit\n") != 0) {
    // wait and read the message from client, copy it in buffer
    int received_size = read(clientfd, buffer, BUFFER_SIZE);
    if (received_size < 0){
        printf("Receiving failed\n");
        exit(EXIT_FAILURE);
    }

    	if (strcmp(buffer, "exit\n") == 0) {
		bzero(buffer, BUFFER_SIZE);
		break;
	}

    // print buffer which contains the client contents
    printf("Receive message from client: %s", buffer);

    // and send that buffer to client
    int ret = write(clientfd, buffer, received_size);
    if (ret < 0){
        printf("Sending failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Send the message back to client: %s", buffer);
	bzero(buffer, BUFFER_SIZE);
	}
	close(clientfd);
	}
    // Close the socket at the end
    //close(clientfd);
    close(sockfd); 
    return;
}

int main(int argc, char* argv[])
{
    int opt;

    unsigned int port = 0;
    while ((opt = getopt(argc, argv, "p:n:")) != -1) {
        switch (opt) {
        case 'p':
            port = atoi(optarg);
            break;
        case 'n':
            the_number = atoi(optarg);
            break;
        default: /* '?' */
            fprintf(stderr, "Server Application Usage: %s -p <port_number>\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (port == 0){
        fprintf(stderr, "ERROR: Port number for server to listen is not given\n");
        fprintf(stderr, "Server Application Usage: %s -p <port_number>\n",
                    argv[0]);
        exit(EXIT_FAILURE);
    }
    run_server(port);

    return 0;
}
