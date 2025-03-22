#ifndef RORAM_H
#define RORAM_H

#include "OramDeterministic.h" // Or "OramReadPathEviction.h" if using that instead
#include <vector>
#include <cmath>

class rORAM {
public:
    // Constructor
    rORAM(UntrustedStorageInterface* storage, RandForOramInterface* rand_gen,
          int bucket_size, int num_blocks, int max_range_size);

    // Destructor
    ~rORAM();

    // Read a range of blocks
    std::vector<Block> ReadRange(int start, int end);

    // Write a block to all sub-ORAMs
    void WriteBlock(int blockIndex, int* newdata);

    // Perform batch evictions across all sub-ORAMs
    void batchEvict(int batch_size);

private:
    int max_range_size; // Maximum range size supported
    int num_sub_orams;  // Number of sub-ORAMs
    std::vector<OramDeterministic*> sub_orams; // List of sub-ORAMs
};

#endif // RORAM_H
