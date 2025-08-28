#ifndef COMPRESSORCONTROLLER_H
#define COMPRESSORCONTROLLER_H

#include "utils.h"
#include "compressor.h"
#include "compressionContext.h"
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

class compressorController : public compressor{
public:
    compressorController();

    void takeInput();     
    void runCompressor(); 
    void writeOutput();   

    void compressData(const string&, const string&) {} 

private:
    // header
    int x_count=0, y_count=0, z_count=0;
    int px=1, py=1, pz=1;

    // tag map: char -> label string
    unordered_map<char,string> tag_to_label;

    // single-line pushback for robust reading
    string pushLine; bool hasPush=false;
    bool readline(string &out);
    void unread(const string &s);

    compressionContext ctx;

    inline string labelOf(char ch) const {
        auto it = tag_to_label.find(ch);
        return (it==tag_to_label.end()? string(1,ch) : it->second);
    }
};

#endif



