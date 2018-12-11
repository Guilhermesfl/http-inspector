#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include "../include/proxy.hpp"
using namespace std;

class spider
{
    private:
    public:
        ofstream spiderFile;
        proxy *proxyRef;
        vector<string> references;
        void createCache();
        void run(string, string, int, int);
        void getPageReference(string, string, int, int, fstream&, vector<string> &);
        int searchLineReference(string, int, string, int, string, int, int, vector<string> &);
        spider();
        ~spider();
};
