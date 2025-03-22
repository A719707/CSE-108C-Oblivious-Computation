#ifndef PORAM_BLOCK_H
#define PORAM_BLOCK_H

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Block {
public:
    static const int BLOCK_SIZE = 2;  // Block data size

    // Constructors
    Block() : index(-1), leaf_id(-1), range_size(0) {
        fill(data, data + BLOCK_SIZE, 0);
    }

    // Backward-compatible constructor
    Block(int leaf_id, int index, int data[]) 
        : index(index), leaf_id(leaf_id), range_size(0) {
        copy(data, data + BLOCK_SIZE, this->data);
        leaf_ids.push_back(leaf_id); // Initialize leaf_ids with leaf_id
    }

    // New constructor for rORAM
    Block(int index, const int data[], const vector<int>& leaf_ids, int range_size = 0) 
        : index(index), leaf_ids(leaf_ids), range_size(range_size) {
        copy(data, data + BLOCK_SIZE, this->data);
        if (!leaf_ids.empty()) {
            leaf_id = leaf_ids[0]; // Initialize leaf_id for backward compatibility
        }
    }

    // Getters and Setters
    int getIndex() const { return index; }
    void setIndex(int idx) { index = idx; }

    const int* getData() const { return data; }
    void setData(const int newData[]) {
        copy(newData, newData + BLOCK_SIZE, data);
    }

    int getLeafId() const { return leaf_id; } // Backward-compatible getter
    void setLeafId(int id) { 
        leaf_id = id; 
        if (leaf_ids.empty()) {
            leaf_ids.push_back(id);
        } else {
            leaf_ids[0] = id;
        }
    }

    const vector<int>& getLeafIds() const { return leaf_ids; }
    void setLeafIds(const vector<int>& newLeafIds) {
        leaf_ids = newLeafIds;
        if (!leaf_ids.empty()) {
            leaf_id = leaf_ids[0]; // Synchronize leaf_id
        }
    }

    int getRangeSize() const { return range_size; }
    void setRangeSize(int size) { range_size = size; }

    // Print function
    void printBlock() const {
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
    virtual ~Block() {}

private:
    int index;               // Logical block index
    int data[BLOCK_SIZE];    // Block data
    int leaf_id;             // Backward-compatible leaf ID
    vector<int> leaf_ids;    // Positions in all sub-ORAMs
    int range_size;          // Size of the range this block belongs to
};

#endif //PORAM_BLOCK_H
