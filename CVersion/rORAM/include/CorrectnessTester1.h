
#ifndef CORRECTNESSTESTER1_H
#define CORRECTNESSTESTER1_H

#include "OramInterface.h"
#include "rORAM.h"
#include <string>

class CorrectnessTester1 {
public:
    CorrectnessTester1();
    void runCorrectnessTest();
    void runRORAMCorrectnessTest(); // New function for testing rORAM

private:
    int* sampleData(int i); // Helper function to generate sample data
};

#endif // CORRECTNESSTESTER1_H
