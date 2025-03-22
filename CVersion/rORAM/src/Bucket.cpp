
#include "Bucket.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

bool Bucket::is_init = false;
int Bucket::max_size = -1;

Bucket::Bucket() {
    if (!is_init) {
        throw new runtime_error("Please set bucket size before creating a bucket");
    }
    blocks = vector<Block>();
}

// Copy constructor
Bucket::Bucket(Bucket *other) {
    if (other == NULL) {
        throw new runtime_error("The other bucket is not allocated.");
    }
    blocks = vector<Block>(max_size);
    for (int i = 0; i < max_size; i++) {
        blocks[i] = Block(other->blocks[i]);
    }
}

// Get block object with matching index
Block Bucket::getBlockByIndex(int index) {
    Block *copy_block = NULL;
    for (Block b : blocks) {
        if (b.getIndex() == index) {
            copy_block = new Block(b);
            break;
        }
    }
    if (copy_block == NULL) {
        throw new runtime_error("Block with index " + to_string(index) + " not found in bucket.");
    }
    return *copy_block;
}

// Add a block to the bucket
void Bucket::addBlock(Block new_blk) {
    if (blocks.size() < max_size) {
        Block toAdd = Block(new_blk);
        blocks.push_back(toAdd);
    } else {
        throw new runtime_error("Bucket is full. Cannot add more blocks.");
    }
}

// Remove a block from the bucket
bool Bucket::removeBlock(Block rm_blk) {
    bool removed = false;
    for (int i = 0; i < blocks.size(); i++) {
        if (blocks[i].getIndex() == rm_blk.getIndex()) {
            blocks.erase(blocks.begin() + i);
            removed = true;
            break;
        }
    }
    return removed;
}

// Get all blocks in a range
vector<Block> Bucket::getBlocksInRange(int start, int end) {
    vector<Block> result;
    for (Block b : blocks) {
        if (b.getIndex() >= start && b.getIndex() < end) {
            result.push_back(b);
        }
    }
    return result;
}

// Perform batch eviction
void Bucket::batchEvict(vector<Block>& evictedBlocks) {
    for (Block b : evictedBlocks) {
        this->addBlock(b);
    }
}

// Return a shallow copy of the blocks in the bucket
vector<Block> Bucket::getBlocks() {
    return this->blocks;
}

// Set the maximum size of the bucket
void Bucket::setMaxSize(int maximumSize) {
    if (is_init) {
        throw new runtime_error("Max Bucket Size was already set");
    }
    max_size = maximumSize;
    is_init = true;
}

// Get the maximum size of the bucket
int Bucket::getMaxSize() {
    return max_size;
}

// Reset the bucket's state
void Bucket::resetState() {
    is_init = false;
}

// Print all blocks in the bucket
void Bucket::printBlocks() {
    for (Block b : blocks) {
        b.printBlock();
    }
}
