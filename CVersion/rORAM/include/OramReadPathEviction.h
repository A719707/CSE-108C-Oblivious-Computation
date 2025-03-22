#ifndef PORAM_ORAMREADPATHEVICTION_H
#define PORAM_ORAMREADPATHEVICTION_H

#include "OramInterface.h"
#include "RandForOramInterface.h"
#include "UntrustedStorageInterface.h"
#include <cmath>
#include <vector>

class OramReadPathEviction : public OramInterface {
public:
    UntrustedStorageInterface* storage;
    RandForOramInterface* rand_gen;

    int bucket_size;
    int num_levels;
    int num_leaves;
    int num_blocks;
    int num_buckets;

    int *position_map; // Array to store position map
    std::vector<Block> stash;

    // Constructor
    OramReadPathEviction(UntrustedStorageInterface* storage,
                         RandForOramInterface* rand_gen, 
                         int bucket_size, 
                         int num_blocks);

    // Access function for single-block operations
    int* access(Operation op, int blockIndex, int newdata[]) override;

    // Helper function to compute bucket index for a given leaf and level
    int P(int leaf, int level);

    // Function to read a range of blocks
    std::vector<Block> ReadRange(int start, int end);

    // Function to perform batch evictions
    void batchEvict(int batch_size);

    // Getter functions
    int* getPositionMap() override;
    std::vector<Block> getStash() override;
    int getStashSize() override;
    int getNumLeaves() override;
    int getNumLevels() override;
    int getNumBlocks() override;
    int getNumBuckets() override;
};

#endif // PORAM_ORAMREADPATHEVICTION_H
