#ifndef PROXY_H
#define PROXY_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#define PROXY_PORT 8228

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
        struct sockaddr_in proxyAddr, cli_addr;
        socklen_t clilen;
        void createSocket(int);
        void createCache();
        void acceptConnection();
        void parseHttp();
        void sendHttpRequest(httpParsed, string);
        bool isCached(string);
        void saveInCache(int);
        proxy();
        ~proxy();
        
};

#endif