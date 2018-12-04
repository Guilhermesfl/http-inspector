#include "aracne.hpp"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iterator>
#include <vector>
#include <netdb.h>
#include <algorithm>
#include <fstream>
#include <streambuf>

#define CLIENT_PORT 80

using namespace std;

/**
 * 
 * Parses Http to extrat some fields
 * 
 * */
httpParsed parseHttp (string bufferRequest) {

	// FILE * request;
	char c, hostURL[150];
	char * hostPosition;
    string method, url, httpVersion, host;
    httpParsed parsedRequest;

    istringstream buf(bufferRequest);
    istream_iterator<string> beg(buf), end;
    vector<string> tokens(beg, end);
    int i = 0;
    for(auto& s: tokens) {
        if (i == 0) parsedRequest.method = s;
        else if (i == 1) parsedRequest.url = s;
        else if (i == 2) parsedRequest.httpVersion = s;
        else if (i == 4) parsedRequest.host = s;
        else if (i > 4) break;
        i++;
    }

    return parsedRequest;
}

/**
 * 
 * Sends http GET request to server
 * 
 * */
void sendHttpRequest(httpParsed request, string bufferRequest) {
    int clientSocket;
    char buffer[4096];
    socklen_t clilen;
    const char *c = request.host.c_str();
    const char *bRequest = bufferRequest.c_str();
    struct sockaddr_in serverAddress;
    struct hostent *server;
    FILE * httpResponse;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket >= 0) cout << "[PROXY] Client socket created on port: " << CLIENT_PORT << endl; 
    else cout << "ERROR opening socket" << endl;

    server = gethostbyname(c);
    if (server) cout << "[PROXY] Server identified..." << endl;
    else {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serverAddress.sin_addr.s_addr,
         server->h_length);
    serverAddress.sin_port = htons(CLIENT_PORT);

    if (connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) >= 0);
    else cout << "[PROXY] Error connecting to server" << endl;

    char newRequest[250] = "GET / HTTP/1.1\r\nHost: ";
    strcat(newRequest, c);
    strcat(newRequest, "\r\n\r\n\r\n");

    if(write(clientSocket, newRequest, strlen(newRequest)) <= 0) cout << "ERROR writing on socket" << endl;

    replace( request.url.begin(), request.url.end(), '/', '_');
    httpResponse = fopen(("responses/"+request.url).c_str(),"w+");
    if(!httpResponse) cout << "ERROR while creating file" << endl;

    bzero(buffer, sizeof(buffer));

    cout << "[PROXY] Receiving response from server" << endl;
    while(recv(clientSocket, buffer, 4095, 0) != 0) //atenção ao tamanho do buffer ***
    {
        cout << ".";
       	fputs(buffer, httpResponse);
       	bzero(buffer, sizeof(buffer));
    }
    cout << "[PROXY] Response received" << endl;
    close(clientSocket);
    fclose(httpResponse);

}

void saveToFile(string toSave, int type, httpParsed parsedHttp) {
	FILE * fp;
	const char *string = toSave.c_str();

    replace( parsedHttp.url.begin(), parsedHttp.url.end(), '/', '_');

	if (type == 1) {
		fp = fopen(("requests/"+parsedHttp.url).c_str(),"w+");
	} else if (type == 2) {
		fp = fopen(("responses/"+parsedHttp.url).c_str(),"w+");
	}

	fputs(string, fp);

	fclose(fp);
}

/**
 * 
 * Function verifies if request has its response cached
 * 
 * */
bool isCached(string requestUrl) {
    
    FILE * fp;

    replace( requestUrl.begin(), requestUrl.end(), '/', '_');
    fp = fopen(("responses/"+requestUrl).c_str(),"r");
    
    if (fp) {
    	fclose(fp);
    	return true;
    } else return false;
    
}

void spider(string filename, string hostname) {
    
    unsigned int currrentLine = 0;
    string line;
    ifstream fp("responses/" + filename);
    FILE *dumpFile = fopen(("spider/" + filename).c_str(),"w+");
    string host = "http://" + hostname + "\n";
    if (fp && dumpFile) {
        fputs(host.c_str(), dumpFile);
        while(getline(fp, line)) { 
            // cout << line << endl;
            currrentLine++;
            if (line.find("http://" + hostname, 0) !=string::npos) {
                int pos = line.find("http://" + hostname, 0), i = 0;
                string href;
                int end = 0;
                for(char& c : line) {
                    if(i >= pos && c != ' ') {
                        href = href + c;
                        i++;
                    }
                    if ( i >= pos && c == ' ') {
                        href = href.substr(0, href.size()-1) + "\n";
                        break;
                    }
                    i++;
                }
                fputs(href.c_str(), dumpFile);
            }
        }
        cout << "[SPIDER] Spider finished!" << endl;
        fp.close();
        fclose(dumpFile);
    } else {
        cout << "[SPIDER] SPIDER error! Could not open files!" << endl;
    }
}