
#include "../include/proxy.hpp"
#include <sys/stat.h>
#define CLIENT_PORT 80
#include <fstream>
#include <sstream>
#include <vector>
#include <netdb.h>
#include <unistd.h>

proxy::proxy() {
    // Proxy config
    this->proxyAddr.sin_family = AF_INET; 
    this->proxyAddr.sin_addr.s_addr = INADDR_ANY; 
    this->proxyAddr.sin_port = htons( PROXY_PORT );
}

proxy::~proxy() {}


/**
 * Creates a server socket to listen for connections
 * */
void proxy::createSocket (int selectedPort) {
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket > 0) cout << "[PROXY] Server socket created on port: " << selectedPort << endl;
    else {
		cout << "[PROXY] Error creating server socket" << endl;
		return;
	}
	int tr=1;
	// kill "Address already in use" error message
    if (setsockopt(this->serverSocket,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    // Bind socket to the port 8228
    if (::bind(this->serverSocket, (struct sockaddr *)&this->proxyAddr, sizeof(this->proxyAddr)) != -1) cout << "[PROXY] Socket successfully binded" << endl;
    else {
		cout << "[PROXY] Error when binding socket" << endl;
		return;
	}

    cout << "[PROXY] Socket has been cofigured" << endl;
	cout << "[PROXY] Socket listening ..." << endl;
	listen(this->serverSocket, 1);
}

void proxy::acceptConnection () {
	        
	// The accept() call actually accepts an incoming connection
	this->clilen = sizeof(this->cli_addr);
	this->clientSocket = accept(this->serverSocket, (struct sockaddr *) &this->cli_addr, &this->clilen);

	if (this->clientSocket > 0) cout << "[PROXY] Received connection..." << endl;
	else cout << "ERROR on accept" << endl;

	int n = recv(this->clientSocket, this->buffer, 4096, 0);
	if (n <= 0) {
		cout << "[PROXY ] Error receiving request from socket" << endl;
		return;
	}
	cout << "[PROXY] Request received: " << endl;
	cout << buffer << endl;
	this->parseHttp();
	this->saveInCache(1);
}

/**
 * Creates response and request cache
 * */
void proxy::createCache () {
    mkdir("../requests", S_IRUSR | S_IWUSR | S_IXUSR);
    mkdir("../responses", S_IRUSR | S_IWUSR | S_IXUSR);
}

/**
 * Verifies if request has its response cached
 * */
bool proxy::isCached(string requestUrl) {
	
	ifstream fp;

	replace( requestUrl.begin(), requestUrl.end(), '/', '_');
	fp.open("../responses/" + requestUrl);
	
	if (fp) {
		fp.close();
		return true;
	} else return false;
	
}

/**
 * Saves response or request to a file
 * */
void proxy::saveInCache(int type) {
	ofstream fp;

	replace( this->parsedRequest.url.begin(), this->parsedRequest.url.end(), '/', '_');

	if (type == 1) {
		fp.open("../requests/" + this->parsedRequest.url);
	} else if (type == 2) {
		fp.open("../responses/" + this->parsedRequest.url);
	}

	fp << this->buffer;
	fp.close();
}

/**
 * 
 * Parses Http to extrat some fields
 * 
 * */
void proxy::parseHttp () {

	// FILE * request;
	char c, hostURL[150];
	char * hostPosition;
	string method, url, httpVersion, host;

	istringstream buf(this->buffer);
	istream_iterator<string> beg(buf), end;
	vector<string> tokens(beg, end);
	int i = 0;
	for(auto& s: tokens) {
		if (i == 0) this->parsedRequest.method = s;
		else if (i == 1) this->parsedRequest.url = s;
		else if (i == 2) this->parsedRequest.httpVersion = s;
		else if (i == 4) this->parsedRequest.host = s;
		else if (i > 4) break;
		i++;
	}
}

/**
 * 
 * Sends http GET request to server
 * 
 * */
void proxy::sendHttpRequest(httpParsed request, string bufferRequest) {
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