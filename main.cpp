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
#include <netdb.h>
#include <fstream>
#include <streambuf>
#define PROXY_PORT 8228

using namespace std;

int main(int argc, char const *argv[])
{
    int defSocket = PROXY_PORT, proxySocket, clientSocket;
    socklen_t clilen;
    char buffer[4096];
    struct sockaddr_in proxyAddr, cli_addr;
    httpParsed parsedHttp;


    if(argv[1] && argv[2]) {
        if (!strcmp(argv[1], "-p")) {
            defSocket = atoi(argv[2]);
        }
    }

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

    // Bind socket to the port 8228
    if (bind(proxySocket, (struct sockaddr *)&proxyAddr, sizeof(proxyAddr)) == 0) cout << "Socket binding: success" << endl;
    else { 
        cout << "Error when binding socket" << endl;
        return -1;
    } 
    cout << "Socket has been cofigured" << endl;

    string c = "n";
    while(1)
    {   
        if (c == "n") {
            httpParsed requestParsed;
            listen(proxySocket,10);
            cout << "[PROXY] Socket listening..." << endl;

            // The accept() call actually accepts an incoming connection
            clilen = sizeof(cli_addr);
            clientSocket = accept(proxySocket, (struct sockaddr *) &cli_addr, &clilen);

            if (clientSocket > 0) cout << "[PROXY] Received connection..." << endl;
            else cout << "ERROR on accept" << endl;

            // Connection received
            // cout << "FROM: " << inet_ntoa(cli_addr.sin_addr) << endl;
            // cout << "PORT: " << ntohs(cli_addr.sin_port) << endl;

            int n = recv(clientSocket, buffer, 4096, 0);
            if (n <= 0) cout << "[PROXY ] Error receiving request from socket" << endl;;
            
            cout << "[PROXY] Request received: " << endl;
            cout << buffer << endl;

            parsedHttp = parseHttp(buffer);

            // saveToFile(buffer, 1, parsedHttp);

            string filename = sendHttpRequest(parsedHttp,buffer);
            ifstream t(filename);
            string str((istreambuf_iterator<char>(t)),
                istreambuf_iterator<char>());
            
            cout << "RESPONSE" << endl;
            cout << str << endl;


            // cout << "PARSED REQUEST: " << endl;
            // cout << requestParsed.host << endl;

            // if (!isCached(parsedHttp.url)) {
            //     cout << "[PROXY] Request not cached. Sending request to server... " << endl;
            //     string teste = sendHttpRequest(parsedHttp,buffer);
            // } else {
            //     cout << "[PROXY] Request found in cache. Responding to client ... " << endl;
            // }
            
            close(clientSocket);
        } else {
            break;
        }
        cin >> c;
    }
    
    close(proxySocket);

    return 0;
}