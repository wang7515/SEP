#ifndef COMPRESSORCONTROLLER_H
#define COMPRESSORCONTROLLER_H

#include "utils.h"
#include "reader.h"
#include "compressor.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class compressorController:public compressor{
    compressorController();
    void compressData(const string &inputFile,const string &outputFile);
    void runCompressor();
    void takeInput();
    void writeOutput();
};
#endif