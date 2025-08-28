#include "compressorController.h"

int main(){
    compressorController ctrl;
    ctrl.takeInput();     
    ctrl.runCompressor(); 
    ctrl.writeOutput();   
    return 0;
}

