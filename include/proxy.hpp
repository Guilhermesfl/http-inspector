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
        int serverSocket, clientSocket;
        struct sockaddr_in proxyAddr, cli_addr;
        void createSocket(int);
        void createCache();
        httpParsed parseHttp(string);
        void sendHttpRequest(httpParsed, string);
        bool isCached(string);
        void saveInCache(string, int, httpParsed);
        proxy();
        ~proxy();
        
};

#endif