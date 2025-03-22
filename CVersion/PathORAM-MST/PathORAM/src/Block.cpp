#include "Block.h"
#include <iostream>
#include <algorithm>

using namespace std;

// Default constructor
Block::Block() : index(-1), leaf_id(-1), range_size(0) {
    fill(data, data + BLOCK_SIZE, 0);
}

// Backward-compatible constructor
Block::Block(int leaf_id, int index, int data[]) 
    : index(index), leaf_id(leaf_id), range_size(0) {
    copy(data, data + BLOCK_SIZE, this->data);
    leaf_ids.push_back(leaf_id); // Initialize leaf_ids with leaf_id
}

// New constructor for rORAM
Block::Block(int index, const int data[], const vector<int>& leaf_ids, int range_size) 
    : index(index), leaf_ids(leaf_ids), range_size(range_size) {
    copy(data, data + BLOCK_SIZE, this->data);
    if (!leaf_ids.empty()) {
        leaf_id = leaf_ids[0]; // Initialize leaf_id for backward compatibility
    }
}

// Print function
void Block::printBlock() const {
    cout << "Block Index: " << index << endl;
    cout << "Data: ";
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        cout << data[i] << " ";
    }
    cout << endl;
    cout << "Leaf ID: " << leaf_id << endl; // Backward-compatible print
    cout << "Leaf IDs: ";
    for (int id : leaf_ids) {
        cout << id << " ";
    }
    cout << endl;
    cout << "Range Size: " << range_size << endl;
}

// Destructor
Block::~Block() {
    // Destructor is trivial; no dynamic memory to clean up
}
