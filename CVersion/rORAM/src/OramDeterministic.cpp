#include "OramDeterministic.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <bitset>
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
OramDeterministic::OramDeterministic(UntrustedStorageInterface* storage, RandForOramInterface* rand_gen,
                                     int bucket_size, int num_blocks) {
    this->G = 0;
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

    // Initialize all blocks in all buckets as empty.
    for (int i = 0; i < num_buckets; i++) {
        Bucket init_bkt = Bucket();
        for (int j = 0; j < bucket_size; j++) {
            init_bkt.addBlock(Block()); // Add dummy block
        }
        storage->WriteBucket(i, init_bkt);
    }
}

// Access function with deterministic eviction and bit-reversed path ordering
int* OramDeterministic::access(Operation op, int blockIndex, int* newdata) {
    int* data = new int[Block::BLOCK_SIZE];
    int oldLeaf = position_map[blockIndex];

    // Update block's new position using bit-reversed order
    uint32_t bitWidth = log2(num_leaves);
    position_map[blockIndex] = getBitReversedIndex(rand_gen->getRandomLeaf(), bitWidth);
    int newLeaf = position_map[blockIndex];

    Block* targetBlock = nullptr;
    for (int i = 0; i < num_levels; i++) {
        vector<Block> blocks = storage->ReadBucket(P(oldLeaf, i)).getBlocks();
        Bucket write_back = Bucket();
        for (Block b : blocks) {
            if (b.index != blockIndex) {
                write_back.addBlock(Block(b));
            } else {
                targetBlock = new Block(b);
                write_back.addBlock(Block());
            }
        }
        storage->WriteBucket(P(oldLeaf, i), write_back);
    }

    if (op == Operation::WRITE) {
        if (targetBlock == nullptr) {
            Block newBlock(newLeaf, blockIndex, newdata);
            stash.push_back(newBlock);
        } else {
            for (int i = 0; i < Block::BLOCK_SIZE; i++) {
                targetBlock->data[i] = newdata[i];
            }
            stash.push_back(Block(*targetBlock));
        }
    } else {
        if (targetBlock == nullptr) {
            data = nullptr;
        } else {
            for (int i = 0; i < Block::BLOCK_SIZE; i++) {
                data[i] = targetBlock->data[i];
            }
            stash.push_back(Block(*targetBlock));
        }
    }

    // Perform batch eviction
    batchEvict(2); // Evict 2 paths deterministically

    return data;
}

// ReadRange function for efficient range queries
std::vector<Block> OramDeterministic::ReadRange(int start, int end) {
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
void OramDeterministic::batchEvict(int batch_size) {
    for (int repeat = 0; repeat < batch_size; repeat++) {
        int g = ReverseBits(this->G, num_levels - 1);
        this->G = this->G + 1;

        for (int l = 0; l < num_levels; l++) {
            vector<Block> blocks = storage->ReadBucket(P(g, l)).getBlocks();
            for (Block b : blocks) {
                if (b.index != -1) {
                    stash.push_back(Block(b));
                }
            }
        }

        for (int l = num_levels - 1; l >= 0; l--) {
            vector<int> bid_evicted;
            Bucket bucket;
            int Pxl = P(g, l);
            int counter = 0;

            for (Block b_instash : stash) {
                if (counter >= bucket_size) {
                    break;
                }
                Block be_evicted = Block(b_instash);
                if (Pxl == P(position_map[be_evicted.index], l)) {
                    bucket.addBlock(be_evicted);
                    bid_evicted.push_back(be_evicted.index);
                    counter++;
                }
            }

            // Remove evicted blocks from the stash
            for (int id : bid_evicted) {
                stash.erase(std::remove(stash.begin(), stash.end(), id), stash.end());
            }

            while (counter < bucket_size) {
                bucket.addBlock(Block()); // Dummy block
                counter++;
            }
            storage->WriteBucket(Pxl, bucket);
        }
    }
}

// Helper function to compute bucket index for a given leaf and level
int OramDeterministic::P(int leaf, int level) {
    return (1 << level) - 1 + (leaf >> (this->num_levels - level - 1));
}

// Reverse bits function for deterministic eviction
int OramDeterministic::ReverseBits(int g, int bits_length) {
    int g_mod = g % num_leaves;
    int reverse = 0;
    while (g_mod) {
        reverse <<= 1;
        reverse |= g_mod & 1;
        g_mod >>= 1;
        bits_length--;
    }
    reverse <<= bits_length;
    return reverse;
}

// Existing getter functions (unchanged)
int* OramDeterministic::getPositionMap() {
    return this->position_map;
}

vector<Block> OramDeterministic::getStash() {
    return this->stash;
}

int OramDeterministic::globalG() {
    return this->G;
}

int OramDeterministic::getStashSize() {
    return (this->stash).size();
}

int OramDeterministic::getNumLeaves() {
    return this->num_leaves;
}

int OramDeterministic::getNumLevels() {
    return this->num_levels;
}

int OramDeterministic::getNumBlocks() {
    return this->num_blocks;
}

int OramDeterministic::getNumBuckets() {
    return this->num_buckets;
}
