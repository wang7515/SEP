#ifndef UTILS_H
#define UTILS_H
#include <string>
using namespace std;

class utils{
public:
    utils();
    bool parseHeader(const string &s, int &x, int &y, int &z, int &px, int &py, int &pz);
};
#endif

