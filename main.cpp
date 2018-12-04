#define _BSD_SOURCE
#include <iostream>
#include "aracne.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fstream>
#include <streambuf>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>
#define PROXY_PORT 8228

using namespace std;

int main(int argc, char const *argv[])
{
    int defSocket = PROXY_PORT, proxySocket, clientSocket;
    socklen_t clilen;
    char buffer[4096];
    struct sockaddr_in proxyAddr, cli_addr;
    httpParsed parsedHttp;

    mkdir("requests", S_IRUSR | S_IWUSR | S_IXUSR);
    mkdir("responses", S_IRUSR | S_IWUSR | S_IXUSR);


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
    int choice = 1;
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

            int n = recv(clientSocket, buffer, 4096, 0);
            if (n <= 0) cout << "[PROXY ] Error receiving request from socket" << endl;;
            
            cout << "[PROXY] Request received: " << endl;
            cout << buffer << endl;

            parsedHttp = parseHttp(buffer);

            // Saves the request to a txt file
            saveToFile(buffer, 1, parsedHttp);

            cout << "CHOOSE WHAT TO DO WITH THE REQUEST:" << endl;
            cout << "1- Send without editing" << endl;
            cout << "2- Edit then send" << endl;

            cin >> choice;

            if (choice == 2) {
                string filename = strcpy((char *) &filename, (char *) &parsedHttp.url);
                replace( filename.begin(), filename.end(), '/', '_');
                filename = "./requests/" + filename;
                system(("nano "+filename).c_str());
            }

            if(!isCached(parsedHttp.url)) {
                cout << "[PROXY] Proxy does not have request cached. Sending request to server ..." << endl;
                sendHttpRequest(parsedHttp,buffer);
                cout << "[PROXY] Response has been cached. Sending response to client ..." << endl;
            } else {
                cout << "[PROXY] Request is cached. Sending to client ..." << endl;
            }
            replace( parsedHttp.url.begin(), parsedHttp.url.end(), '/', '_');
            ifstream t(parsedHttp.url);
            string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
            send(clientSocket , str.c_str() , strlen(str.c_str()) , 0 );
            cout << "Data sent to client" << endl;
            // cout << str << endl;
            // saveToFile(buffer, 1, parsedHttp);
            // cout << "PARSED REQUEST: " << endl;
            // cout << requestParsed.host << endl;
            
            close(clientSocket);
        } else {
            break;
        }
        cin >> c;
    }
    
    close(proxySocket);

    return 0;
}