#include "CorrectnessTester2.h"
#include "Bucket.h"
#include "ServerStorage.h"
#include "OramInterface.h"
#include "RandForOramInterface.h"
#include "RandomForOram.h"
#include "UntrustedStorageInterface.h"
#include "OramReadPathEviction.h"
#include "OramDeterministic.h"
#include "rORAM.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

CorrectnessTester2::CorrectnessTester2() {
}

int* CorrectnessTester2::sampleData(int i) {
    int* newArray = new int[Block::BLOCK_SIZE];
    for (int j = 0; j < Block::BLOCK_SIZE; ++j) {
        newArray[j] = i;
    }
    return newArray;
}

void CorrectnessTester2::runCorrectnessTest() {
    int bucketSize = 2;
    int numBlocks = pow(2, 4);
    Bucket::setMaxSize(bucketSize);

    UntrustedStorageInterface* storage = new ServerStorage();
    RandForOramInterface* random = new RandomForOram();
    OramInterface* oram = new OramReadPathEviction(storage, random, bucketSize, numBlocks);

    cout << "Warming up the stash by writing blocks" << endl;

    // Write blocks to ORAM
    for (int i = 0; i < numBlocks; i++) {
        if (i % 1000 == 0) {
            cout << "Writing Block " << i << " to ORAM. The stash size is: " << to_string(oram->getStashSize()) << std::endl;
        }
        int* accessed = oram->access(OramInterface::Operation::WRITE, i % numBlocks, this->sampleData(i));
    }

    // Update blocks in ORAM
    for (int i = 0; i < numBlocks; i++) {
        if (i % 1000 == 0) {
            cout << "Writing Block " << i << " to ORAM. The stash size is: " << to_string(oram->getStashSize()) << std::endl;
        }
        int* accessed = oram->access(OramInterface::Operation::WRITE, i % numBlocks, this->sampleData((i + 1) % numBlocks));
    }

    // Update blocks in ORAM again
    for (int i = 0; i < numBlocks; i++) {
        if (i % 1000 == 0) {
            cout << "Writing Block " << i << " to ORAM. The stash size is: " << to_string(oram->getStashSize()) << std::endl;
        }
        int* accessed = oram->access(OramInterface::Operation::WRITE, i % numBlocks, this->sampleData((i + 2) % numBlocks));
    }

    // Read blocks from ORAM and verify correctness
    for (int i = 0; i < numBlocks; i++) {
        int* accessed = oram->access(OramInterface::Operation::READ, i, NULL);
        string holder = "";
        int* correct_data = sampleData((i + 2) % numBlocks);
        if (accessed == NULL) {
            cout << "Reading Block " << i << " from ORAM. Block not found!" << std::endl;
        } else {
            for (unsigned int j = 0; j < Block::BLOCK_SIZE; ++j) {
                int temp = accessed[j];
                if (correct_data[j] != temp) {
                    cout << "Expected and found values don't match!" << " Accessed = " << temp << " Correct = " << correct_data[j] << endl;
                }
                holder += to_string(temp);
                holder += " ";
            }
            cout << "Reading Block " << i << " from ORAM. Value is : " << holder << std::endl;
            cout << "Stash size is " << to_string(oram->getStashSize()) << std::endl;
        }
    }

    delete oram;
    delete random;
    delete storage;
}

void CorrectnessTester2::runRORAMCorrectnessTest() {
    int bucketSize = 2;
    int numBlocks = pow(2, 4);
    int maxRangeSize = 16; // Maximum range size supported by rORAM
    Bucket::setMaxSize(bucketSize);

    UntrustedStorageInterface* storage = new ServerStorage();
    RandForOramInterface* random = new RandomForOram();
    rORAM oram(storage, random, bucketSize, numBlocks, maxRangeSize);

    cout << "Warming up the stash by writing blocks" << endl;

    // Write blocks to rORAM
    for (int i = 0; i < numBlocks; i++) {
        if (i % 1000 == 0) {
            cout << "Writing Block " << i << " to rORAM." << std::endl;
        }
        oram.WriteBlock(i % numBlocks, this->sampleData(i));
    }

    // Update blocks in rORAM
    for (int i = 0; i < numBlocks; i++) {
        if (i % 1000 == 0) {
            cout << "Writing Block " << i << " to rORAM." << std::endl;
        }
        oram.WriteBlock(i % numBlocks, this->sampleData((i + 1) % numBlocks));
    }

    // Update blocks in rORAM again
    for (int i = 0; i < numBlocks; i++) {
        if (i % 1000 == 0) {
            cout << "Writing Block " << i << " to rORAM." << std::endl;
        }
        oram.WriteBlock(i % numBlocks, this->sampleData((i + 2) % numBlocks));
    }

    // Read blocks from rORAM and verify correctness
    for (int i = 0; i < numBlocks; i++) {
        std::vector<Block> blocks = oram.ReadRange(i, i + 1); // Read a single block
        if (blocks.empty()) {
            cout << "Reading Block " << i << " from rORAM. Block not found!" << std::endl;
        } else {
            string holder = "";
            int* correct_data = sampleData((i + 2) % numBlocks);
            for (unsigned int j = 0; j < Block::BLOCK_SIZE; ++j) {
                int temp = blocks[0].getData()[j];
                if (correct_data[j] != temp) {
                    cout << "Expected and found values don't match!" << " Accessed = " << temp << " Correct = " << correct_data[j] << endl;
                }
                holder += to_string(temp);
                holder += " ";
            }
            cout << "Reading Block " << i << " from rORAM. Value is : " << holder << std::endl;
        }
    }

    // Test range queries
    cout << "Testing range queries with rORAM..." << endl;
    std::vector<Block> rangeBlocks = oram.ReadRange(0, 5); // Read a range of 5 blocks
    for (Block& block : rangeBlocks) {
        string holder = "";
        for (unsigned int j = 0; j < Block::BLOCK_SIZE; ++j) {
            int temp = block.getData()[j];
            holder += to_string(temp);
            holder += " ";
        }
        cout << "Block " << block.getIndex() << " in range. Value is : " << holder << std::endl;
    }

    // Perform batch eviction
    cout << "Performing batch eviction..." << endl;
    oram.batchEvict(10);

    delete random;
    delete storage;
}
