#ifndef READER_H
#define READER_H

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

class reader{
    private:
        int parentBlockSize;
        int blockDivisor;
    public:
        reader();
        void readParentBlock();
        void returnChunk();
};
#endif
