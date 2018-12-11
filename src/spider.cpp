#include "../include/spider.hpp"
#include "../include/proxy.hpp"
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#define PROXY_PORT 8228

using namespace std;

spider::spider() {}

spider::~spider() {}

void spider::createCache () {
    mkdir("../spider", S_IRUSR | S_IWUSR | S_IXUSR);
}

/**
 * Spider function
 * */
void spider::run(string filename, string hostname, int treeHeight, int actualHeight) {
	string url = filename;
	replace( url.begin(), url.end(), '_', '/');
	vector<string> references;
	references.push_back(url);
	if (actualHeight < treeHeight){
		fstream responseFile;
		this->proxyRef->sendHttpRequest(filename, hostname);
		responseFile.open("../responses/" + filename);
		if(responseFile.is_open()) {
			getPageReference(filename, hostname, actualHeight, treeHeight, responseFile, references);
		} else {
			cout << "Error opening response file" << endl;
			return;
		}
		responseFile.close();
		if (actualHeight != 0) {
			string cmd = "rm ../responses/" + filename;
			system(cmd.c_str());
		}
	}
	
}

/**
 * Get page references
 * */
void spider::getPageReference(string filename, string hostname, int actualHeight, int treeHeight,
	fstream& requestFile, vector<string> &references) {
	unsigned int currrentLine = 0, position = 0;
	string line;
	while(getline(requestFile, line)) { 
		currrentLine++;
		while (line.find("href=",  position) != string::npos) {
			position = searchLineReference("href=", 6, line, position, hostname, actualHeight, treeHeight, references);
		}
		position = 0;
		while (line.find("src=\"", position) != string::npos) {
			position = searchLineReference("src=\"", 5 ,line, position, hostname, actualHeight, treeHeight, references);
		}
	}
}

/**
 * Search line reference
 * */
int spider::searchLineReference(string searchString, int offset, string line, int position, 
	string hostname, int actualHeight, int treeHeight, vector<string> &references) {
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
		if (value.find("http://" + hostname) != string::npos || line[pos+offset] == '/') {
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
				for(int i = 0; i < actualHeight + 1; i++) {
					this->spiderFile << "  ";
				}
				this->spiderFile << value << endl;
				references.push_back(value);
				if (value.find(".jpg") == string::npos && value.find(".png") == string::npos
				&& value.find(".ico") == string::npos && value.find(".gif") == string::npos
				&& value.find(".js") == string::npos && value.find(".css") == string::npos) {
					replace( value.begin(), value.end(), '/', '_');
					this->run(value, hostname, treeHeight, actualHeight+1);
				}
				
			}
			return i + 1;
		} else {
			return pos + offset;  
		}
	} else {
		return pos + offset;  
	}
}
