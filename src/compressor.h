#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <string>
using namespace std;

class compressor{
public:                           
    compressor() = default;       
    virtual ~compressor() = default;

    virtual void runCompression() {}   

    string chooseAlgorithm();
};

#endif
