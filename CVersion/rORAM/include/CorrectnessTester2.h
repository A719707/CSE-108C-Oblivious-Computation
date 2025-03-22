
#ifndef CORRECTNESSTESTER2_H
#define CORRECTNESSTESTER2_H

#include "OramInterface.h"
#include "rORAM.h"
#include <string>

class CorrectnessTester2 {
public:
    CorrectnessTester2();
    void runCorrectnessTest();
    void runRORAMCorrectnessTest(); // New function for testing rORAM

private:
    int* sampleData(int i); // Helper function to generate sample data
};

#endif // CORRECTNESSTESTER2_H
