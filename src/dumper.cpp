#include <iostream>
#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include "../include/dumper.hpp"
#include <unistd.h>
#include <algorithm>
using namespace std;

dumper::dumper(/* args */) {}

dumper::~dumper() {}

void dumper::createCache () {
    mkdir("../dumps", S_IRUSR | S_IWUSR | S_IXUSR);
}

void dumper::createDumpDir(string filename) {
    string path = "../dumps/" + filename;
    mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
}

void dumper::run(string filename) {
	unsigned int currrentLine = 0, position = 0;
	string line;
    int i = 0;
	while(getline(this->spiderFile, line)) {
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
        this->dump(line, "../dumps/" + filename);
	}
}

void dumper::dump(string url, string filename) {
    fstream file;
    size_t n = std::count(url.begin(), url.end(), '/');
    if (n > 2) {
        string path = filename + this->getSubPath(url);
        // cout << path;
        mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
        this->dump(this->getRemainingPath(url), path);
    } else if (n == 2) {
        string path = filename + this->getSubPath(url);
        // cout << path;
        mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
        if (url[url.length() - 1] == '/') {
            file.open(filename + "/index.html",ios::out);
            if (file) file.close();
        } else {
            file.open(path + this->getRemainingPath(url),ios::out);
            if (file) file.close();
        }
        return;
    } else {
        string path = filename + url;
        file.open(filename+ url,ios::out);
        if (file) file.close();
        return;
    }
}

string dumper::getRemainingPath(string url) {
    for(int i = 1; i < url.length(); i++)
	{
		if (url[i] == '/') {
			url = url.substr(i, url.length());
			return url;
		}
	}
}

string dumper::getSubPath(string url) {
    int i;
    for(int i = 1; i < url.length(); i++) {
        if (url[i] == '/' || i == url.length()) {
            url = url.substr(0, i);
            return url;
        }
    }
}