#ifndef PROXY_H
#define PROXY_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#define PROXY_PORT 8228
#define CLIENT_PORT 80

using namespace std;

class httpParsed {
    public:
        string method;
        string url;
        string host;
        string httpVersion; 
};

class proxy {
    private:
    public:
        char buffer[4096];
        int serverSocket, clientSocket;
        httpParsed parsedRequest;
        struct sockaddr_in proxyAddr, cli_addr, serverAddr;
        socklen_t clilen;
        struct hostent *server;
        void createSocket(int);
        void createCache();
        void acceptConnection();
        void parseHttp();
        void sendHttpRequest(string, string, int);
        bool isCached();
        void saveInCache(int);
        void editHttp(int);
        proxy();
        ~proxy();
        
};

#endif