#ifndef PORAM_BLOCK_H
#define PORAM_BLOCK_H

#include <algorithm>
#include <iostream>

using namespace std;

#define MAX_ORAMS 10 // Maximum number of sub-ORAMs, adjust based on rORAM depth

class Block {
public:
    static const int BLOCK_SIZE = 2;  // Block data size
    int index;                        // Logical block index
    int data[BLOCK_SIZE];             // Block data
    int leaf_ids[MAX_ORAMS];          // Positions in all sub-ORAMs

    // Constructors
    Block();  
    Block(int index, int data[], int leaf_ids[]);

    // Print function
    void printBlock();

    // Destructor
    virtual ~Block();
};

#endif // PORAM_BLOCK_H
