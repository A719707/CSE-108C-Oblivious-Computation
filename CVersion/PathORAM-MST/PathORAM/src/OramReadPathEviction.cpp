#include "OramReadPathEviction.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

// Utility function to get bit-reversed index
uint32_t getBitReversedIndex(uint32_t index, uint32_t bitWidth) {
    uint32_t reversed = 0;
    for (uint32_t i = 0; i < bitWidth; i++) {
        if (index & (1 << i))
            reversed |= (1 << (bitWidth - 1 - i));
    }
    return reversed;
}

// Constructor
OramReadPathEviction::OramReadPathEviction(UntrustedStorageInterface* storage, RandForOramInterface* rand_gen,
                                           int bucket_size, int num_blocks) {
    this->storage = storage;
    this->rand_gen = rand_gen;
    this->bucket_size = bucket_size;
    this->num_blocks = num_blocks;
    this->num_levels = ceil(log2(num_blocks)) + 1;
    this->num_buckets = pow(2, num_levels) - 1;

    if (this->num_buckets * this->bucket_size < this->num_blocks) {
        throw runtime_error("Not enough space for the actual number of blocks.");
    }

    this->num_leaves = pow(2, num_levels - 1);
    Bucket::resetState();
    Bucket::setMaxSize(bucket_size);
    this->rand_gen->setBound(num_leaves);
    this->storage->setCapacity(num_buckets);
    this->position_map = new int[this->num_blocks];
    this->stash = vector<Block>();

    // Initialize position map with bit-reversed leaf positions
    uint32_t bitWidth = log2(num_leaves);
    for (int i = 0; i < this->num_blocks; i++) {
        position_map[i] = getBitReversedIndex(rand_gen->getRandomLeaf(), bitWidth);
    }

    // Initialize storage with dummy blocks
    for (int i = 0; i < num_buckets; i++) {
        Bucket init_bkt = Bucket();
        for (int j = 0; j < bucket_size; j++) {
            init_bkt.addBlock(Block()); // Add dummy block
        }
        storage->WriteBucket(i, init_bkt);
    }
}

// Access function with batch eviction and bit-reversed path ordering
int* OramReadPathEviction::access(Operation op, int blockIndex, int *newdata) {
    int *data = new int[Block::BLOCK_SIZE];
    int oldLeaf = position_map[blockIndex];

    // Update block's new position using bit-reversed order
    uint32_t bitWidth = log2(num_leaves);
    position_map[blockIndex] = getBitReversedIndex(rand_gen->getRandomLeaf(), bitWidth);
    int newLeaf = position_map[blockIndex];

    // Read path (bit-reversed ordering)
    vector<Block> pathBlocks;
    for (int i = 0; i < num_levels; i++) {
        int pathIndex = getBitReversedIndex(P(oldLeaf, i), bitWidth);
        vector<Block> blocks = storage->ReadBucket(pathIndex).getBlocks();
        for (Block& b : blocks) {
            if (b.index != -1) {
                stash.push_back(b);
            }
        }
    }

    // Find the target block in the stash
    Block* targetBlock = nullptr;
    int targetPos = -1;
    for (int i = 0; i < stash.size(); i++) {
        if (stash[i].index == blockIndex) {
            targetBlock = &stash[i];
            targetPos = i;
            break;
        }
    }

    // Perform read or write operation
    if (op == Operation::WRITE) {
        if (targetBlock == nullptr) {
            Block newBlock(newLeaf, blockIndex, newdata);
            stash.push_back(newBlock);
        } else {
            for (int i = 0; i < Block::BLOCK_SIZE; i++) {
                targetBlock->data[i] = newdata[i];
            }
        }
    } else {
        if (targetBlock != nullptr) {
            for (int i = 0; i < Block::BLOCK_SIZE; i++) {
                data[i] = targetBlock->data[i];
            }
        } else {
            delete[] data;
            return nullptr;
        }
    }

    // Perform batch eviction
    batchEvict(batch_size);

    return data;
}

// Helper function to compute bucket index for a given leaf and level
int OramReadPathEviction::P(int leaf, int level) {
    return (1 << level) - 1 + (leaf >> (this->num_levels - level - 1));
}

// ReadRange function for efficient range queries
std::vector<Block> OramReadPathEviction::ReadRange(int start, int end) {
    std::vector<Block> result;
    int range_size = end - start;

    // Determine the sub-ORAM index based on the range size
    int sub_oram_index = std::ceil(std::log2(range_size));

    // Fetch blocks from the appropriate sub-ORAM
    for (int i = start; i < end; i++) {
        int leaf_id = position_map[i];
        int bitWidth = log2(num_leaves);
        int reversedLeaf = getBitReversedIndex(leaf_id, bitWidth);

        // Read the path for the block
        for (int l = 0; l < num_levels; l++) {
            int pathIndex = getBitReversedIndex(P(reversedLeaf, l), bitWidth);
            Bucket bucket = storage->ReadBucket(pathIndex);
            std::vector<Block> blocks = bucket.getBlocks();

            // Add blocks in the range to the result
            for (Block& block : blocks) {
                if (block.getIndex() >= start && block.getIndex() < end) {
                    result.push_back(block);
                }
            }
        }
    }

    // Update the position map for the blocks in the range
    for (int i = start; i < end; i++) {
        position_map[i] = getBitReversedIndex(rand_gen->getRandomLeaf(), log2(num_leaves));
    }

    return result;
}

// Batch eviction function
void OramReadPathEviction::batchEvict(int batch_size) {
    // Collect blocks from the stash that need to be evicted
    std::vector<Block> blocks_to_evict;
    for (int i = 0; i < stash.size() && i < batch_size; i++) {
        blocks_to_evict.push_back(stash[i]);
    }

    // Determine eviction paths using bit-reversed ordering
    std::vector<int> eviction_paths;
    for (int i = 0; i < batch_size; i++) {
        int leaf_id = rand_gen->getRandomLeaf();
        int bitWidth = log2(num_leaves);
        int reversedLeaf = getBitReversedIndex(leaf_id, bitWidth);
        eviction_paths.push_back(reversedLeaf);
    }

    // Perform evictions in batches
    for (int path : eviction_paths) {
        for (int l = num_levels - 1; l >= 0; l--) {
            int pathIndex = getBitReversedIndex(P(path, l), bitWidth);
            Bucket bucket;

            // Add blocks to the bucket
            for (Block& block : blocks_to_evict) {
                if (bucket.getBlocks().size() < bucket_size) {
                    bucket.addBlock(block);
                }
            }

            // Fill remaining slots with dummy blocks
            while (bucket.getBlocks().size() < bucket_size) {
                bucket.addBlock(Block());
            }

            // Write the bucket back to storage
            storage->WriteBucket(pathIndex, bucket);
        }
    }

    // Remove evicted blocks from the stash
    for (Block& block : blocks_to_evict) {
        stash.erase(std::remove(stash.begin(), stash.end(), block), stash.end());
    }
}

// Existing helper functions (unchanged)
int* OramReadPathEviction::getPositionMap() {
    return this->position_map;
}

vector<Block> OramReadPathEviction::getStash() {
    return this->stash;
}

int OramReadPathEviction::getStashSize() {
    return (this->stash).size();
}

int OramReadPathEviction::getNumLeaves() {
    return this->num_leaves;
}

int OramReadPathEviction::getNumLevels() {
    return this->num_levels;
}

int OramReadPathEviction::getNumBlocks() {
    return this->num_blocks;
}

int OramReadPathEviction::getNumBuckets() {
    return this->num_buckets;
}
