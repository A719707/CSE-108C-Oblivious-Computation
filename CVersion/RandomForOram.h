#ifndef PORAM_RANDOMFORORAM_H
#define PORAM_RANDOMFORORAM_H

#include <cmath>
#include <cstdlib>
#include <vector>
#include <random>
#include <iostream>
#include "RandForOramInterface.h"
#include "duthomhas/csprng.hpp"

using namespace std;

class RandomForOram : public RandForOramInterface {
public:
    // Static members
    static bool is_initialized;
    static int bound;

    // Member variables
    vector<int> rand_history;
    linear_congruential_engine<unsigned long, 25214903917, 11, 281474976710656> rnd_generator;
    std::mt19937 mt_generator;
    long seed = 0L;
    duthomhas::csprng rng;

    // Constructors
    RandomForOram();
    RandomForOram(int seed); // Constructor with seed for deterministic testing

    // Random number generation methods
    int getRandomLeaf();
    int getRandomLeafMT(); // Mersenne Twister-based random leaf
    int getRandomLeafLCG(); // Linear Congruential Generator-based random leaf

    // Configuration methods
    void setBound(int totalNumOfLeaves);
    void resetState();
    void clearHistory();

    // Debugging and history methods
    vector<int> getHistory();
};

// Static member initialization
bool RandomForOram::is_initialized = false;
int RandomForOram::bound = -1;

#endif // PORAM_RANDOMFORORAM_H
