#ifndef ORAMDETERMINISTIC_H
#define ORAMDETERMINISTIC_H

#include "UntrustedStorageInterface.h"
#include "RandForOramInterface.h"
#include "Bucket.h"
#include "Block.h"
#include <vector>
#include <stdexcept>
#include <cmath>

using namespace std;

class OramDeterministic {
public:
    enum Operation { READ, WRITE };

    // Constructor
    OramDeterministic(UntrustedStorageInterface* storage, RandForOramInterface* rand_gen,
                      int bucket_size, int num_blocks);

    // Access function
    int* access(Operation op, int blockIndex, int *newdata);

    // Helper functions
    int ReverseBits(int g, int bits_length);
    int P(int leaf, int level);

    // Getters
    vector<int>* getPositionMap();
    vector<Block> getStash();
    int globalG();
    int getStashSize();
    int getNumLeaves();
    int getNumLevels();
    int getNumBlocks();
    int getNumBuckets();

private:
    int G;  // Global counter for eviction
    UntrustedStorageInterface* storage;
    RandForOramInterface* rand_gen;
    int bucket_size;
    int num_blocks;
    int num_levels;
    int num_buckets;
    int num_leaves;
    vector<int>* position_map;  // Array of vectors for multiple sub-ORAMs
    vector<Block> stash;
};

#endif // ORAMDETERMINISTIC_H
