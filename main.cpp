#define _BSD_SOURCE
#include <iostream>
#include "aracne.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#define PROXY_PORT 8228

using namespace std;

int main(int argc, char const *argv[])
{
    int defSocket = PROXY_PORT, proxySocket, clientSocket;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in proxyAddr, cli_addr;;

    if(argv[1] && argv[2]) {
        if (!strcmp(argv[1], "-p")) {
            defSocket = atoi(argv[2]);
        }
    }
    
    // First step - Create a socket
    // AF_INET - IPv4 protocol
    // SOCK_STREAM - TCP
    // 0 - IP protocol
    proxySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxySocket <= 0) {
        cout << "Error when creating socket";
        return -1;
    };

    proxyAddr.sin_family = AF_INET; 
    proxyAddr.sin_addr.s_addr = INADDR_ANY; 
    proxyAddr.sin_port = htons( PROXY_PORT );

    // Binds socket to the port 8080
    if (bind(proxySocket, (struct sockaddr *)&proxyAddr, sizeof(proxyAddr)) < 0) { 
        cout << "Error when binding socket";
        return -1;
    } 

    listen(proxySocket,5);

    // The accept() call actually accepts an incoming connection
    clilen = sizeof(cli_addr);
    clientSocket = accept(proxySocket, (struct sockaddr *) &cli_addr, &clilen);
    if (clientSocket < 0) cout << "ERROR on accept" << endl;

    printf("server: got connection from %s port %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));


    // This send() function sends the 13 bytes of the string to the new socket
    send(clientSocket, "Hello, world!\n", 13, 0);

    bzero(buffer,256);

    int n = read(clientSocket,buffer,255);
    if (n < 0) cout << "ERROR reading from socket" << endl;;
    printf("Here is the message: %s\n",buffer);

    close(proxySocket);
    close(clientSocket);

    return 0;
}