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
#include <vector>
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
    mkdir("spider", S_IRUSR | S_IWUSR | S_IXUSR);
    mkdir("dumps", S_IRUSR | S_IWUSR | S_IXUSR);


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

    int tr=1;

    // kill "Address already in use" error message
    if (setsockopt(proxySocket,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

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
            if (n <= 0) {
                cout << "[PROXY ] Error receiving request from socket" << endl;
                break;
            }
            
            cout << "[PROXY] Request received: " << endl;
            cout << buffer << endl;

            parsedHttp = parseHttp(buffer);

            // Saves the request to a txt file
            saveToFile(buffer, 1, parsedHttp);

            cout << "[PROXY] CHOOSE WHAT TO DO WITH THE REQUEST:" << endl;
            cout << "1- Send without editing" << endl;
            cout << "2- Edit then send" << endl;

            cin >> choice;

            if (choice == 2) {
                string filename = parsedHttp.url;
                replace( filename.begin(), filename.end(), '/', '_');
                string cmd = "vim requests/" + filename; 
                const char *c = cmd.c_str();
                system(c);
                ifstream t("requests/" + filename);
                string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
                sprintf(buffer, "%.4s", str.c_str());
            }

            if(!isCached(parsedHttp.url)) {
                cout << "[PROXY] Proxy does not have request cached. Sending request to server ..." << endl;
                sendHttpRequest(parsedHttp,buffer);
                cout << "[PROXY] Response has been cached. Sending response to client ..." << endl;
            } else {
                cout << "[PROXY] Request is cached. Sending to client ..." << endl;
            }
            
            replace( parsedHttp.url.begin(), parsedHttp.url.end(), '/', '_');

            cout << "[PROXY] CHOOSE WHAT TO DO WITH THE RESPONSE:" << endl;
            cout << "1- Return to client without editing" << endl;
            cout << "2- Edit then send" << endl;
            cout << "3- SPIDER" << endl;

            cin >> choice;

            
            if (choice == 2 || choice == 1) {
                if (choice == 2) {
                    string filename = parsedHttp.url;
                    replace( filename.begin(), filename.end(), '/', '_');
                    string cmd = "vim responses/" + filename; 
                    const char *c = cmd.c_str();
                    system(c);
                }
                ifstream t("responses/" + parsedHttp.url);
                string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
                send(clientSocket , str.c_str() , strlen(str.c_str()) , 0 );
                cout << "Data sent to client" << endl;
            } else {
                string filename = parsedHttp.url;
                replace( filename.begin(), filename.end(), '/', '_');
                vector<string> references;
                references.push_back("http://" + parsedHttp.host + "/");
                cout << "[SPIDER] Running..." << endl;
                spider(filename, parsedHttp.host, 1, 0, references);
            }
            close(clientSocket);
        } else {
            break;
        }
        cin >> c;
    }

    close(proxySocket);

    return 0;
}