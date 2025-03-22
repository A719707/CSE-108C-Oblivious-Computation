
#ifndef CORRECTNESSTESTER4_H
#define CORRECTNESSTESTER4_H

#include "OramInterface.h"
#include "UntrustedStorageInterface.h"
#include "RandForOramInterface.h"
#include "Block.h"
#include <iostream>
#include <string>
#include <cmath>

class CorrectnessTester4 {
public:
    CorrectnessTester4();
    void runCorrectnessTest(); // Runs the correctness test for ORAM

private:
    int* sampleData(int i); // Generates sample data for a block
};

#endif // CORRECTNESSTESTER4_H
