#include "aracne.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iterator>
#include <vector>
#include <netdb.h>

#define CLIENT_PORT 80

using namespace std;

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

	// request = fopen("request.txt","w+");
    // method = bufferRequest.substr(0, bufferRequest.find(' '));
	//fputs(&bufferRequest[0], request);

	// hostPosition = strstr(&bufferRequest[0], "www.");

	// while(*(hostPosition) != '\n' && *(hostPosition) != '\0')
	// {
	// 	strncat(hostURL, hostPosition, 1);
	// 	hostPosition++;
	// }

	// fputs(&hostURL[0], request);

	// fclose(request);

    return parsedRequest;
}

string sendHttpRequest(httpParsed request, string bufferRequest) {
    int clientSocket;
    char buffer[4096];
    socklen_t clilen;
    const char *c = request.host.c_str();
    const char *url = request.url.c_str();
    const char *bRequest = bufferRequest.c_str();
    struct sockaddr_in serverAddress;
    struct hostent *server;
    FILE * httpResponse;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket >= 0) cout << "[PROXY] Client socket create on port: " << CLIENT_PORT << endl; 
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

    if(write(clientSocket, bRequest, strlen(bRequest)) > 0) cout << "[PROXY] Sucessfully written on socket." << endl;
    else cout << "ERROR writing on socket";

    httpResponse = fopen("response.txt","w+");
    if(httpResponse == NULL) cout << "ERROR AO CRIAR ARQUIVO" << endl;

    while(1)
    {
    	bzero(buffer, sizeof(buffer));

    	int n = recv(clientSocket, buffer, 4096, 0);

        if (n <= 0)
        {
        	cout << "[PROXY] End of response transmission" << endl;
        	break;
        }
        else fputs(buffer, httpResponse);


    }
    
    // printf("Please enter the message: ");
    // bzero(buffer,256);
    // fgets(buffer,255,stdin);
    // n = write(clientSocket, buffer, strlen(buffer));
    // if (n < 0) 
    //      error("ERROR writing to socket");
    // bzero(buffer,256);
    // n = read(clientSocket, buffer, 255);
    // if (n < 0) 
    //      error("ERROR reading from socket");
    // printf("%s\n", buffer);
    close(clientSocket);
    fclose(httpResponse);

    return 0;

}