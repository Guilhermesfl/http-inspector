#include <iostream>
#include <string.h>
#include <vector>

using namespace std;

class spider
{
    private:
    public:
        void createCache();
        void run(string, string, int, int, vector<string>&, ofstream&);
        void getPageReference(string, string, vector<string> &, int, int, fstream&, ofstream&);
        int searchLineReference(string, int, string, int, ofstream&, string, vector<string> &, int, int);
        spider();
        ~spider();
};

spider::spider() {}

spider::~spider() {}
