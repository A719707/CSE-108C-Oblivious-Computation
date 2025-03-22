
#ifndef CORRECTNESSTESTER3_H
#define CORRECTNESSTESTER3_H

#include "OramInterface.h"
#include "rORAM.h"
#include <string>

class CorrectnessTester3 {
public:
    CorrectnessTester3();
    void runCorrectnessTest();
    void runRORAMCorrectnessTest(); // New function for testing rORAM

private:
    int* sampleData(int i); // Helper function to generate sample data
};

#endif // CORRECTNESSTESTER3_H
