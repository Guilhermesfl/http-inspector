#include <iostream>
#include <string.h>
#include <fstream>

using namespace std;

class dumper
{
    private:
    public:
        fstream spiderFile;
        string hostname;
        void createCache();
        void createDumpDir(string);
        void dump(string, string);
        string getRemainingPath(string);
        string getSubPath(string);
        string getPath(string);
        void run(string);
        dumper(/* args */);
        ~dumper();
};
