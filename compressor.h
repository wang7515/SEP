#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class compressor{
    compressor();
    virtual void runCompression();
    string chooseAlgorithm();
};
#endif