#include "spider.hpp"
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#define PROXY_PORT 8228

using namespace std;

void spider::createCache () {
    mkdir("spider", S_IRUSR | S_IWUSR | S_IXUSR);
}

/**
 * Spider function
 * */
void spider::run(string filename, string hostname, int treeHeight, int actualHeight,
    vector<string> &references, ofstream& spiderFile) {
	
	char buffer[4096];
	httpParsed parsedHttp;
	fstream requestFile;
	parsedHttp.url = filename;
	parsedHttp.host = hostname;
	if (actualHeight != treeHeight){
		sendHttpRequest(parsedHttp, buffer);
		requestFile.open("responses/" + filename);
		if(requestFile.is_open()) {
			cout << references[1] << endl;
			getPageReference(filename, hostname, references, actualHeight, treeHeight, requestFile, spiderFile);
		} else {
			cout << "Error opening response file" << endl;
		}
		requestFile.close();
	}
	
}

/**
 * Get page references
 * */
void spider::getPageReference(string filename, string hostname, vector<string> &references, int actualHeight, int treeHeight,
	fstream& requestFile, ofstream& spiderFile) {
	unsigned int currrentLine = 0, position = 0;
	string line;
	spiderFile << hostname << endl;
	while(getline(requestFile, line)) { 
		currrentLine++;
		while (line.find("href=", position) != string::npos) {
			position = searchLineReference("href=", 6, line, position, spiderFile, hostname, references, actualHeight, treeHeight);
		}
		position = 0;
		while (line.find("src=\"", position) != string::npos) {
			position = searchLineReference("src=\"", 5 ,line, position, spiderFile, hostname, references, actualHeight, treeHeight);
		}
	}
}

/**
 * Search line reference
 * */
int spider::searchLineReference(string searchString, int offset, string line, int position, 
	ofstream& spiderFile, string hostname, vector<string> &references, int actualHeight, int treeHeight) {
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
				replace( value.begin(), value.end(), '/', '_');
				spider(value, hostname, treeHeight, actualHeight+1, references, spiderFile);
				references.push_back(value);
				// spiderFile << value << endl;
			}
			return i + 1;
		} else {
			return pos + offset;  
		}
	} else {
		return pos + offset;  
	}
}
