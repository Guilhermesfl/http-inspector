#define _BSD_SOURCE
// #include "dumper.hpp"
// #include "spider.hpp"
#include "../include/proxy.hpp"
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <sys/types.h>
#include <algorithm>
#include <vector>

using namespace std;

int main(int argc, char const *argv[])
{
    proxy webProxy;
    // spider *webSpider;
    // dumper *webDumper;
    int selectedPort = PROXY_PORT;
    socklen_t clilen;
    char buffer[4096];
    httpParsed parsedHttp;

    if(argv[1] && argv[2]) {
        if (!strcmp(argv[1], "-p")) {
            selectedPort = atoi(argv[2]);
        }
    }
    
    // Configure proxy cache and client socket
    webProxy.createCache();
    webProxy.createSocket(selectedPort);
    // Configure spider cache
    // webSpider->createCache();
    // Configure dumper cache
    // webDumper->createCache();

    // while(1)
    // {   
    //     if (c == "n") {
    //         httpParsed requestParsed;
    //         listen(proxySocket,10);
    //         cout << "[PROXY] Socket listening..." << endl;

    //         // The accept() call actually accepts an incoming connection
    //         clilen = sizeof(cli_addr);
    //         clientSocket = accept(proxySocket, (struct sockaddr *) &cli_addr, &clilen);

    //         if (clientSocket > 0) cout << "[PROXY] Received connection..." << endl;
    //         else cout << "ERROR on accept" << endl;

    //         int n = recv(clientSocket, buffer, 4096, 0);
    //         if (n <= 0) {
    //             cout << "[PROXY ] Error receiving request from socket" << endl;
    //             break;
    //         }
    //         parsedHttp = parseHttp(buffer);
            



            
    //         cout << "[PROXY] Request received: " << endl;
    //         cout << buffer << endl;
    //         saveToFile(buffer, 1, parsedHttp);
    //         cout << "[PROXY] CHOOSE WHAT TO DO WITH THE REQUEST:" << endl;
    //         cout << "1- Send without editing" << endl;
    //         cout << "2- Edit then send" << endl;

    //         cin >> choice;

    //         if (choice == 2) {
    //             string filename = parsedHttp.url;
    //             replace( filename.begin(), filename.end(), '/', '_');
    //             string cmd = "vim requests/" + filename; 
    //             const char *c = cmd.c_str();
    //             system(c);
    //             ifstream t("requests/" + filename);
    //             string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
    //             sprintf(buffer, "%.4s", str.c_str());
    //         }

    //         if(!isCached(parsedHttp.url)) {
    //             cout << "[PROXY] Proxy does not have request cached. Sending request to server ..." << endl;
    //             sendHttpRequest(parsedHttp,buffer);
    //             cout << "[PROXY] Response has been cached. Sending response to client ..." << endl;
    //         } else {
    //             cout << "[PROXY] Request is cached. Sending to client ..." << endl;
    //         }
            
    //         replace( parsedHttp.url.begin(), parsedHttp.url.end(), '/', '_');

    //         cout << "[PROXY] CHOOSE WHAT TO DO WITH THE RESPONSE:" << endl;
    //         cout << "1- Return to client without editing" << endl;
    //         cout << "2- Edit then send" << endl;
    //         cout << "3- SPIDER" << endl;

    //         cin >> choice;

            
    //         if (choice == 2 || choice == 1) {
    //             if (choice == 2) {
    //                 string filename = parsedHttp.url;
    //                 replace( filename.begin(), filename.end(), '/', '_');
    //                 string cmd = "vim responses/" + filename; 
    //                 const char *c = cmd.c_str();
    //                 system(c);
    //             }
    //             ifstream t("responses/" + parsedHttp.url);
    //             string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
    //             send(clientSocket , str.c_str() , strlen(str.c_str()) , 0 );
    //             cout << "Data sent to client" << endl;
    //         } else {
    //             cout << "[PROXY] Input tree height: ";
    //             cin >> choice;
    //             vector<string> references;
    //             string filename = parsedHttp.url;
    //             references.push_back("http://" + parsedHttp.host + "/");
    //             replace( filename.begin(), filename.end(), '/', '_');
    //             cout << "[SPIDER] Running..." << endl;
    //             ofstream spiderFile;
    //             spiderFile.open("spider/" + filename);
    //             spider(filename, parsedHttp.host, choice, 0, references, spiderFile);
    //         }
    //         close(clientSocket);
    //     } else {
    //         break;
    //     }
    //     cin >> c;
    // }

    close(webProxy.serverSocket);

    return 0;
}