
#ifndef CORRECTNESSTESTER5_H
#define CORRECTNESSTESTER5_H

#include "OramInterface.h"
#include "UntrustedStorageInterface.h"
#include "RandForOramInterface.h"
#include "Block.h"
#include <iostream>
#include <string>
#include <cmath>

class CorrectnessTester5 {
public:
    CorrectnessTester5();
    void runCorrectnessTest(); // Runs the correctness test for ORAM

private:
    int* sampleData(int i); // Generates sample data for a block
};

#endif // CORRECTNESSTESTER5_H
