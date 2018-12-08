#include "dumper.hpp"
#include <sys/stat.h>

using namespace std;

void dumper::createCache () {
    mkdir("dumps", S_IRUSR | S_IWUSR | S_IXUSR);
}