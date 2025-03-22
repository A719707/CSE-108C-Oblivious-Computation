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
    Block() : index(-1), range_size(0) {
        fill(data, data + BLOCK_SIZE, 0);
    }

    Block(int index, const int data[], const vector<int>& leaf_ids, int range_size = 0) 
        : index(index), leaf_ids(leaf_ids), range_size(range_size) {
        copy(data, data + BLOCK_SIZE, this->data);
    }

    // Getters and Setters
    int getIndex() const { return index; }
    void setIndex(int idx) { index = idx; }

    const int* getData() const { return data; }
    void setData(const int newData[]) {
        copy(newData, newData + BLOCK_SIZE, data);
    }

    const vector<int>& getLeafIds() const { return leaf_ids; }
    void setLeafIds(const vector<int>& newLeafIds) {
        leaf_ids = newLeafIds;
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
    vector<int> leaf_ids;    // Positions in all sub-ORAMs
    int range_size;          // Size of the range this block belongs to
};

#endif // PORAM_BLOCK_H
