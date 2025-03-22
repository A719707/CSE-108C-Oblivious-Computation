#ifndef BUCKET_H
#define BUCKET_H

#include "Block.h"
#include <vector>
#include <stdexcept>

using namespace std;

class Bucket {
public:
    // Static variables
    static bool is_init;
    static int max_size;

    // Constructors
    Bucket();
    Bucket(Bucket *other);

    // Block management
    Block getBlockByIndex(int index);
    void addBlock(Block new_blk);
    bool removeBlock(Block rm_blk);

    // Getter for blocks
    vector<Block> getBlocks();

    // Static methods for bucket size
    static void setMaxSize(int maximumSize);
    static int getMaxSize();
    static void resetState();

    // Print function
    void printBlocks();

private:
    vector<Block> blocks;  // Blocks stored in the bucket
};

#endif // BUCKET_H
