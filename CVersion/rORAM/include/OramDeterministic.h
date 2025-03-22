#ifndef PORAM_ORAMDETERMINISTIC_H
#define PORAM_ORAMDETERMINISTIC_H

#include "OramInterface.h"
#include "OramReadPathEviction.h"
#include <vector>

class OramDeterministic : public OramInterface {
public:
    UntrustedStorageInterface* storage;
    RandForOramInterface* rand_gen;
    int G;
    int bucket_size;
    int num_levels;
    int num_leaves;
    int num_blocks;
    int num_buckets;
    int *position_map; // Array to store position map
    std::vector<Block> stash;

    // Constructor
    OramDeterministic(UntrustedStorageInterface* storage,
                      RandForOramInterface* rand_gen,
                      int bucket_size,
                      int num_blocks);

    // Access function for single-block operations
    int* access(Operation op, int block_id, int newdata[]) override;

    // Function to read a range of blocks
    std::vector<Block> ReadRange(int start, int end);

    // Function to perform batch evictions
    void batchEvict(int batch_size);

    // Helper function to compute bucket index for a given leaf and level
    int P(int leaf, int level);

    // Reverse bits function for deterministic eviction
    int ReverseBits(int G, int bits_length);

    // Getter functions
    int* getPositionMap() override;
    std::vector<Block> getStash() override;
    int globalG();
    int getStashSize() override;
    int getNumLeaves() override;
    int getNumLevels() override;
    int getNumBlocks() override;
    int getNumBuckets() override;

    // Utility function to convert a number to base 2 (unchanged)
    static std::string ConvertToBase2(int num);
};

#endif // PORAM_ORAMDETERMINISTIC_H
