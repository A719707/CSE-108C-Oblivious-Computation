#include "rORAM.h"
#include <vector>
#include <cmath>

// Constructor
rORAM::rORAM(UntrustedStorageInterface* storage, RandForOramInterface* rand_gen,
             int bucket_size, int num_blocks, int max_range_size) {
    this->max_range_size = max_range_size;
    this->num_sub_orams = ceil(log2(max_range_size)) + 1;

    // Initialize sub-ORAMs
    for (int i = 0; i < num_sub_orams; i++) {
        sub_orams.push_back(new OramDeterministic(storage, rand_gen, bucket_size, num_blocks));
    }
}

// Destructor
rORAM::~rORAM() {
    for (auto& sub_oram : sub_orams) {
        delete sub_oram;
    }
}

// Read a range of blocks
std::vector<Block> rORAM::ReadRange(int start, int end) {
    int range_size = end - start;
    int sub_oram_index = ceil(log2(range_size));

    // If the range size is not a power of 2, split the query
    if ((1 << sub_oram_index) != range_size) {
        int mid = start + (1 << (sub_oram_index - 1));
        std::vector<Block> result1 = sub_orams[sub_oram_index]->ReadRange(start, mid);
        std::vector<Block> result2 = sub_orams[sub_oram_index - 1]->ReadRange(mid, end);
        result1.insert(result1.end(), result2.begin(), result2.end());
        return result1;
    }

    // Use the appropriate sub-ORAM to fetch the range
    return sub_orams[sub_oram_index]->ReadRange(start, end);
}

// Write a block to all sub-ORAMs
void rORAM::WriteBlock(int blockIndex, int* newdata) {
    for (auto& sub_oram : sub_orams) {
        sub_oram->access(OramInterface::WRITE, blockIndex, newdata);
    }
}

// Perform batch evictions across all sub-ORAMs
void rORAM::batchEvict(int batch_size) {
    for (auto& sub_oram : sub_orams) {
        sub_oram->batchEvict(batch_size);
    }
}
