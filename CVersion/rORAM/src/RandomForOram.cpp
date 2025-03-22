#include "RandomForOram.h"
#include <iostream>
#include <bitset> // For bit manipulation

bool RandomForOram::is_initialized = false;
int RandomForOram::bound = -1;

RandomForOram::RandomForOram() {
    if (this->is_initialized) {
        throw new runtime_error("ONLY ONE RANDOM INSTANCE CAN BE USED AT A TIME");
    }
    this->rng(std::random_device{}());
    this->rand_history = std::vector<int>();
    is_initialized = true;
}

// Generate a random leaf using the default RNG
int RandomForOram::getRandomLeaf() {
    long next = this->rng();
    int randVal = next % this->bound;
    if (randVal < 0) {
        randVal += this->bound;
    }
    return randVal;
}

// Generate a random leaf using Mersenne Twister RNG
int RandomForOram::getRandomLeafMT() {
    long next = mt_generator();
    int randVal = next % this->bound;
    this->rand_history.push_back(randVal);
    return randVal;
}

// Generate a random leaf using Linear Congruential Generator (LCG)
int RandomForOram::getRandomLeafLCG() {
    long next = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
    this->seed = next;
    int bits = next >> (48 - 31);
    int randVal = 0;
    if ((this->bound & -this->bound) == this->bound) {  // i.e., bound is a power of 2
        randVal = (int)((this->bound * (long)bits) >> 31);
        return randVal;
    }
    int val = bits % this->bound;
    while (bits - val + (bound - 1) < 0) {
        next = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
        this->seed = next;
        bits = next >> (48 - 31);
        val = bits % this->bound;
    }
    randVal = val;
    this->rand_history.push_back(randVal);
    return randVal;
}

// Generate a bit-reversed path for rORAM
int RandomForOram::getBitReversedPath(int path) {
    int reversed = 0;
    int bits = std::log2(this->bound); // Number of bits required to represent the bound
    for (int i = 0; i < bits; i++) {
        reversed <<= 1;
        reversed |= (path & 1);
        path >>= 1;
    }
    return reversed;
}

// Set the bound for random number generation
void RandomForOram::setBound(int totalNumOfLeaves) {
    this->bound = totalNumOfLeaves;
}

// Reset the initialization state
void RandomForOram::resetState() {
    this->is_initialized = false;
}

// Clear the history of generated random numbers
void RandomForOram::clearHistory() {
    this->rand_history = std::vector<int>();
}

// Get the history of generated random numbers
std::vector<int> RandomForOram::getHistory() {
    return this->rand_history;
}
