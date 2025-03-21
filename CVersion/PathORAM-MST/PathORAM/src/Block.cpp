
#include "Block.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

// Default constructor for a dummy block
Block::Block() {
    this->index = -1;
    for (int i = 0; i < MAX_ORAMS; i++) {
        this->leaf_ids[i] = -1; // Initialize all ORAM leaf positions as invalid
    }
}

// Constructor with data and multiple ORAM leaf positions
Block::Block(int index, int data[], int leaf_ids[]) : index(index) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        this->data[i] = data[i];
    }
    for (int i = 0; i < MAX_ORAMS; i++) {
        this->leaf_ids[i] = leaf_ids[i]; // Store leaf positions for all sub-ORAMs
    }
}

// Destructor
Block::~Block() {}

// Print block information
void Block::printBlock() {
    string data_holder = "";
    for (int i = 0; i < BLOCK_SIZE; i++) {
        data_holder += to_string(this->data[i]) + " ";
    }

    cout << "index: " << to_string(this->index) << " leaf_ids: ";
    
    // Print leaf positions for all sub-ORAMs
    for (int i = 0; i < MAX_ORAMS; i++) {
        cout << leaf_ids[i] << " ";
    }

    cout << " data: " << data_holder << endl;
}
