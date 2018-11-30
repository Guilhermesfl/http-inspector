#include <iostream>
#include "aracne.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <netinet/in.h>
#define PROXY_PORT 8228

using namespace std;

int main(int argc, char const *argv[])
{
    int defSocket = PROXY_PORT, proxySocket;
    struct sockaddr_in proxyAddr;

    if(argv[1] && argv[2]) {
        if (!strcmp(argv[1], "-p")) {
            defSocket = atoi(argv[2]);
        }
    }
    
    // Creates socket
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


    return 0;
}