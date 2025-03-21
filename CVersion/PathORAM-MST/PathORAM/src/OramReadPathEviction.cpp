
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

    // **Batch Eviction: Write multiple paths at once**
    for (int l = num_levels - 1; l >= 0; l--) {
        vector<int> evictedBlockIDs;
        Bucket bucket;
        int pathIndex = getBitReversedIndex(P(oldLeaf, l), bitWidth);
        int counter = 0;

        for (Block& b : stash) {
            int mappedPath = getBitReversedIndex(P(position_map[b.index], l), bitWidth);
            if (counter < bucket_size && mappedPath == pathIndex) {
                bucket.addBlock(b);
                evictedBlockIDs.push_back(b.index);
                counter++;
            }
        }

        // Remove evicted blocks from stash
        for (int id : evictedBlockIDs) {
            stash.erase(remove_if(stash.begin(), stash.end(),
                                 [id](Block& b) { return b.index == id; }),
                        stash.end());
        }

        // Fill remaining slots with dummy blocks
        while (counter < bucket_size) {
            bucket.addBlock(Block());
            counter++;
        }

        // Write evicted bucket back to storage
        storage->WriteBucket(pathIndex, bucket);
    }

    return data;
}

// Helper function to compute bucket index for a given leaf and level
int OramReadPathEviction::P(int leaf, int level) {
    return (1 << level) - 1 + (leaf >> (this->num_levels - level - 1));
}
