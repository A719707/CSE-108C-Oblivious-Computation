#pragma once

#include "UntrustedStorageInterface.h"
#include "RandForOramInterface.h"
#include "Bucket.h"
#include "Block.h"
#include <vector>

enum class Operation { READ, WRITE };

class OramReadPathEviction {
private:
    UntrustedStorageInterface* storage;
    RandForOramInterface* rand_gen;

    int bucket_size;
    int num_blocks;
    int num_levels;
    int num_buckets;
    int num_leaves;

    std::vector<int> position_map; 
    std::vector<Block> stash;

public:
    // Constructor
    OramReadPathEviction(UntrustedStorageInterface* storage, RandForOramInterface* rand_gen, 
                         int bucket_size, int num_blocks);

    // Access a block (READ/WRITE) and return data read (if applicable)
    std::vector<int> access(Operation op, int blockIndex, std::vector<int> newdata = {});

    // Deterministic function to compute the index of a bucket at a given level
    int P(int leaf, int level) const;

    // Getter functions
    std::vector<int> getPositionMap() const;
    std::vector<Block> getStash() const;
    int getStashSize() const;
    int getNumLeaves() const;
    int getNumLevels() const;
    int getNumBlocks() const;
    int getNumBuckets() const;
};
