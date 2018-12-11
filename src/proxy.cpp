
#include "../include/proxy.hpp"
#include <sys/stat.h>
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
	listen(this->serverSocket, 10);
}

/**
 * Opens client socket to receive incoming connections
 * */
void proxy::acceptConnection () {
	        
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
bool proxy::isCached() {
	
	ifstream fp;
	string filename = this->parsedRequest.url;
	replace( filename.begin(), filename.end(), '/', '_');
	
	fp.open("../responses/" + filename);
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
	string filename = this->parsedRequest.url;
	replace( filename.begin(), filename.end(), '/', '_');

	if (type == 1) {
		fp.open("../requests/" + filename);
	} else if (type == 2) {
		fp.open("../responses/" + filename);
	}

	fp << this->buffer;
	fp.close();
}

/**
 * Saves response or request to a file
 * */
void proxy::editHttp(int type) {
	ifstream t;
	string filename = this->parsedRequest.url;
	string cmd;
	replace( filename.begin(), filename.end(), '/', '_');
	
	if (type) cmd = "vim ../requests/" + filename; 
	else cmd = "vim ../responses/" + filename;
	const char *c = cmd.c_str();
		
	system(c);
	if (type) t.open("../requests/" + filename);
	else t.open("../responses/" + filename);

	string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
	sprintf(this->buffer, "%.4s", str.c_str());
	
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
void proxy::sendHttpRequest(string filename, string hostname) {
	FILE * httpResponse;

	this->clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->clientSocket >= 0) cout << "[PROXY] Client socket created on port: " << CLIENT_PORT << endl; 
	else {
		cout << "[PROXY] ERROR opening socket" << endl;
		return;
	}
	this->server = gethostbyname(hostname.c_str());
	if (this->server) cout << "[PROXY] Server identified..." << endl;
	else {
		fprintf(stderr,"[PROXY] ERROR, no such host\n");
		exit(0);
	}
	this->serverAddr.sin_family = AF_INET; 
	bzero((char *) &this->serverAddr, sizeof(this->serverAddr));
	bcopy((char *) this->server->h_addr, (char *)&this->serverAddr.sin_addr.s_addr, this->server->h_length);
    this->serverAddr.sin_port = htons( CLIENT_PORT );

	if (connect(this->clientSocket, (struct sockaddr *) &this->serverAddr, sizeof(this->serverAddr)) >= 0);
	else {
		cout << "[PROXY] Error connecting to server" << endl;
		return;
	}
	string path = filename;
	replace( path.begin(), path.end(), '_', '/');
	
	for(int i = 7; i < path.length(); i++)
	{
		if (path[i] == '/') {
			path = path.substr(i, path.length() - 1);
			break;
		}
		
	}
	path = "GET " + path + " HTTP/1.1\r\nHost: " + hostname + "\r\n\r\n\r\n";
	if(write(this->clientSocket, path.c_str(), path.length()) <= 0) cout << "ERROR writing on socket" << endl;
	replace( filename.begin(), filename.end(), '/', '_');
	httpResponse = fopen(("../responses/" + filename).c_str(),"w+");

	if(!httpResponse) cout << "[PROXY] ERROR while creating file" << endl;

	bzero(this->buffer, sizeof(this->buffer));

	cout << "[PROXY] Receiving response from server" << endl;
	while(recv(this->clientSocket, this->buffer, 4095, 0) != 0)
	{
		cout << ".";
	   	fputs(this->buffer, httpResponse);
	   	bzero(this->buffer, sizeof(this->buffer));
	}
	cout << endl << "[PROXY] Response received" << endl;
	
	close(this->clientSocket);	
	fclose(httpResponse);
}