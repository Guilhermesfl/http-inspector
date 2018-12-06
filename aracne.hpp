#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

class httpParsed {
    public:
        string method;
        string url;
        string host;
        string httpVersion; 
};

httpParsed parseHttp(string bufferRequest);
void sendHttpRequest(httpParsed request, string bufferRequest);
void saveToFile(string toSave, int type, httpParsed parsedHttp);
bool isCached(string requestUrl);
void spider(string filename, string hostname, int treeHeight, int actualHeight, vector<string> &references);
void getPageReference(string filename, string hostname, vector<string> &references, int i, int treeHeight);
int searchLineReference(string searchString, int offset, string line, int position, ofstream& spiderFile, string hostname, vector<string> &references);