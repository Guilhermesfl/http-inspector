#define _BSD_SOURCE
#include "../include/dumper.hpp"
#include "../include/spider.hpp"
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
    spider webSpider;
    string filename;
    dumper webDumper;
    int selectedPort = PROXY_PORT, choice, c = 1;
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
    webSpider.createCache();
    // Configure dumper cache
    webDumper.createCache();
    while(1) {
        if (c) {
            cout << "[PROXY] Waiting for connection..." << endl;
            webProxy.acceptConnection();
            cout << "[PROXY] CHOOSE WHAT TO DO WITH THE REQUEST:" << endl;
            cout << "1 - Send without editing" << endl;
            cout << "2 - Edit then send" << endl;
            cout << "3 - Block request" << endl;
            cout << "CHOICE: ";
            cin >> choice;

            
            if (choice == 1 || choice == 2) {
            
                if (choice == 2) webProxy.editHttp(1);

                if(!webProxy.isCached()) {
                    cout << "[PROXY] Proxy does not have response cached. Sending request to server ..." << endl;
                    webProxy.sendHttpRequest(webProxy.parsedRequest.url, webProxy.parsedRequest.host);
                } else {
                    cout << "[PROXY] Response is cached. Sending to client ..." << endl;
                }
                cout << "[PROXY] CHOOSE WHAT TO DO WITH THE RESPONSE:" << endl;
                cout << "1 - Send without editing" << endl;
                cout << "2 - Edit then send" << endl;
                cout << "3 - Run SPIDER" << endl;
                cout << "4 - Run DUMPER" << endl;
                cin >> choice;
                
                filename = webProxy.parsedRequest.url;
                replace( filename.begin(), filename.end(), '/', '_');
                if (choice == 1 || choice == 2) {
                    if (choice == 2) webProxy.editHttp(2);
                    ifstream t(filename);
                    string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
                    send(webProxy.clientSocket , str.c_str() , strlen(str.c_str()), 0);
                    cout << "[PROXY] Data sent to client" << endl;
                } else if (choice == 3) {
                    cout << "[SPIDER] Input tree height: ";
                    cin >> choice;
                    cout << "[SPIDER] Running..." << endl;
                    webSpider.spiderFile.open("../spider/" + filename);
                    string aux = webProxy.parsedRequest.url;
                    for(int i = 7; i < aux.length(); i++)
                    {
                        if (aux[i] == '/') {
                            aux = aux.substr(i, aux.length() - 1);
                            break;
                        }
                    }
                    webSpider.spiderFile << aux << endl;
                    webSpider.proxyRef = &webProxy;
                    webSpider.run(filename, webProxy.parsedRequest.host, choice, 0, 0);
                    webSpider.spiderFile.close();
                    cout << "[SPIDER] Finished running... Check the file under spider folder to see result!" << endl;
                } else {
                    cout << "[DUMPER] Running..." << endl;
                    webDumper.createDumpDir(webProxy.parsedRequest.host);
                    webDumper.spiderFile.open("../spider/" + filename);
                    webDumper.run(webProxy.parsedRequest.host);
                    webSpider.spiderFile.close();
                    cout << "[SPIDER] Finished running... Check the dumps folder to see result!" << endl;
                }
                close(webProxy.clientSocket);
            } else {
                cout << "[PROXY] Proxy blocked request!" << endl;
            }
            cin >> c;
        } else {
            cout << "[PROXY] Received command to stop ... Goodbye!" << endl;
            break;
        }
    }

    close(webProxy.serverSocket);

    return 0;
}