//
//
//

#ifndef ORAMREADPATHEVICTION_H
#define ORAMREADPATHEVICTION_H

#include <vector>
#include "UntrustedStorageInterface.h"
#include "RandForOramInterface.h"
#include "Block.h"
#include "Bucket.h"
#include <cmath>
#include <stdexcept>

class OramReadPathEviction {
public:
    // Constructor
    OramReadPathEviction(UntrustedStorageInterface* storage, RandForOramInterface* rand_gen, int bucket_size, int num_blocks);

    // Method
    int* access(Operation op, int blockIndex, int *newdata);

private:
    UntrustedStorageInterface* storage;
    RandForOramInterface* rand_gen;
    int bucket_size;
    int num_blocks;
    int num_levels;
    int num_buckets;
    int num_leaves;
    int* position_map;
    std::vector<Block> stash;

    // Helper method to calculate position
    int P(int leaf, int level) {
        return (1 << level) - 1 + (leaf >> (num_levels - level - 1));
    }
};

#endif // ORAMREADPATHEVICTION_H
