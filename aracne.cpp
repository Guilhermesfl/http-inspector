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
	cout << endl << "[PROXY] Response received" << endl;
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

void spider(string filename, string hostname, int treeHeight, int actualHeight, vector<string> &references) {
	
	// sendHttpRequest(parsedHttp, buffer);
	if (actualHeight != treeHeight) {
		cout << references[1] << endl;
		getPageReference(filename, hostname, references, actualHeight, treeHeight);
	}
	
}

void getPageReference(string filename, string hostname, vector<string> &references, int actualHeight, int treeHeight) {
	unsigned int currrentLine = 0, position = 0;
	string line;
	fstream requestFile;
	ofstream spiderFile;
	requestFile.open("responses/" + filename);
	spiderFile.open("spider/" + filename);
	if (requestFile.is_open() && spiderFile.is_open()) {
		spiderFile << hostname << endl;
		while(getline(requestFile, line)) { 
			currrentLine++;
			while (line.find("href=", position) != string::npos) {
				position = searchLineReference("href=", 6, line, position, spiderFile, hostname, references, actualHeight);
			}
			position = 0;
			while (line.find("src=\"", position) != string::npos) {
				position = searchLineReference("src=\"", 5 ,line, position, spiderFile, hostname, references, actualHeight);
			}
		}
	} else {
		cout << "Error opening response file" << endl;
	}
	
	requestFile.close();
}

int searchLineReference(string searchString, int offset, string line, int position, 
	ofstream& spiderFile, string hostname, vector<string> &references, int actualHeight) {
	string value;
	int pos = line.find(searchString, position), i = 0;
	if (line[pos+offset] != '#') {
		for(i = pos + offset; ; i++)
		{
			if(i >= pos && line[i] != '"') value = value + line[i];
			else if ( i >= pos && line[i] == '"') {
				break;
			}
		}
		if (value.find(hostname) != string::npos || line[pos+offset] == '/') {
			if (line[pos+offset] == '/') value = "http://" + hostname + value;
			int hasValue = 0;
			for(int i = 0; i < references.size() ; i++)
			{
				if (references[i] == value) {
					hasValue = 1;
					break;
				}
			}
			if (!hasValue) {
				cout << "	" << value << endl;
				spider(spiderFile, hostname, 1, actualHeight+1, references);
				references.push_back(value);
				spiderFile << value << endl;
			}
			return i + 1;
		} else {
			return pos + offset;  
		}
	} else {
		return pos + offset;  
	}
	
}