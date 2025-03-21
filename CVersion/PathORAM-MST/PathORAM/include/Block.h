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
    Block();  // Default constructor
    Block(int index, const int data[], const vector<int>& leaf_ids);  // Parameterized constructor

    // Getters and Setters
    int getIndex() const;
    void setIndex(int idx);

    const int* getData() const;
    void setData(const int newData[]);

    const vector<int>& getLeafIds() const;
    void setLeafIds(const vector<int>& newLeafIds);

    // Print function
    void printBlock() const;

    // Destructor
    virtual ~Block();

private:
    int index;               // Logical block index
    int data[BLOCK_SIZE];    // Block data
    vector<int> leaf_ids;    // Positions in all sub-ORAMs
};

#endif // PORAM_BLOCK_H
