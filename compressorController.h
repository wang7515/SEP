#ifndef COMPRESSORCONTROLLER_H
#define COMPRESSORCONTROLLER_H

#include "utils.h"
#include "reader.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class compressorController{
    compressorController();
    void compressData(const string &inputFile,const string &outputFile);
};
#endif