#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class utils{
    utils();
    bool parseHeader(const string &s,int &x,int &y,int &z,int &px,int &py,int &pz);
};
#endif