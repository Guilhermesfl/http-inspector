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
    char buffer[4096];
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
    if (proxySocket > 0) cout << "Socket created on port: " << defSocket << endl;
    else {
        cout << "Error creating socket" << endl;
        return -1;
    };

    // Proxy config
    proxyAddr.sin_family = AF_INET; 
    proxyAddr.sin_addr.s_addr = INADDR_ANY; 
    proxyAddr.sin_port = htons( PROXY_PORT );

    // Bind socket to the port 8080
    if (bind(proxySocket, (struct sockaddr *)&proxyAddr, sizeof(proxyAddr)) == 0) cout << "Socket binding: success" << endl;
    else { 
        cout << "Error when binding socket" << endl;
        return -1;
    } 
    cout << "Socket has been cofigured" << endl;

    
    while(1)
    {        
        listen(proxySocket,10);
        cout << "Socket listening" << endl;

        // The accept() call actually accepts an incoming connection
        clilen = sizeof(cli_addr);
        clientSocket = accept(proxySocket, (struct sockaddr *) &cli_addr, &clilen);

        if (clientSocket > 0) cout << "Proxy received connection" << endl;
        else cout << "ERROR on accept" << endl;

        // Connection received
        cout << "FROM: " << inet_ntoa(cli_addr.sin_addr) << endl;
        cout << "PORT: " << ntohs(cli_addr.sin_port) << endl;

        int n = recv(clientSocket, buffer, 4096, 0);
        if (n > 0) cout << "Socket received message" << endl;
        else cout << "ERROR reading from socket" << endl;;
        
        cout << "MESSAGE: " << endl;
        //cout << buffer << endl;

        //string request(buffer);
        cout << "PARSED REQUEST: " << endl;
        //cout << request << endl;
        //cout << parseHttp(request) << endl;
        parseHttp(buffer);

        close(clientSocket);
    }
    
    close(proxySocket);

    return 0;
}