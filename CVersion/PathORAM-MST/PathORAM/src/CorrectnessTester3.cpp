
#include "CorrectnessTester3.h"
#include "Bucket.h"
#include "ServerStorage.h"
#include "OramInterface.h"
#include "RandForOramInterface.h"
#include "RandomForOram.h"
#include "UntrustedStorageInterface.h"
#include "OramReadPathEviction.h"
#include "rORAM.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

CorrectnessTester3::CorrectnessTester3() {
}

int* CorrectnessTester3::sampleData(int i) {
    int* newArray = new int[Block::BLOCK_SIZE];
    for (int j = 0; j < Block::BLOCK_SIZE; ++j) {
        newArray[j] = i;
    }
    return newArray;
}

void CorrectnessTester3::runCorrectnessTest() {
    int bucketSize = 2;
    int numBlocks = pow(2, 12);
    Bucket::setMaxSize(bucketSize);

    UntrustedStorageInterface* storage = new ServerStorage();
    RandForOramInterface* random = new RandomForOram();
    OramInterface* oram = new OramReadPathEviction(storage, random, bucketSize, numBlocks);

    cout << "Warming up the stash by writing blocks" << endl;

    // Write blocks to ORAM
    for (int i = 0; i < 30000; i++) {
        int* accessed = oram->access(OramInterface::Operation::WRITE, i % numBlocks, this->sampleData(i));
    }

    // Update blocks in ORAM
    for (int i = 0; i < 5 * numBlocks; i++) {
        int* accessed = oram->access(OramInterface::Operation::WRITE, i % numBlocks, this->sampleData(i));
        if (i % 1000 == 0) {
            cout << "Writing Block " << i << " to ORAM. The stash size is: " << to_string(oram->getStashSize()) << std::endl;
        }
    }

    // Read blocks from ORAM and verify correctness
    for (int i = 0; i < numBlocks; i++) {
        int* accessed = oram->access(OramInterface::Operation::READ, i, NULL);
        string holder = "";
        int* correct_data = sampleData(4 * numBlocks + i);
        for (unsigned int j = 0; j < Block::BLOCK_SIZE; ++j) {
            int temp = accessed[j];
            if (correct_data[j] != temp) {
                cout << "Expected and found values don't match!" << endl;
            }
            holder += to_string(temp);
            holder += " ";
        }
        if (i % 1000 == 0) {
            cout << "Reading Block " << i << " from ORAM. Value is : " << holder << std::endl;
            cout << "Stash size is " << to_string(oram->getStashSize()) << std::endl;
        }
    }

    delete oram;
    delete random;
    delete storage;
}

void CorrectnessTester3::runRORAMCorrectnessTest() {
    int bucketSize = 2;
    int numBlocks = pow(2, 12);
    int maxRangeSize = 16; // Maximum range size supported by rORAM
    Bucket::setMaxSize(bucketSize);

    UntrustedStorageInterface* storage = new ServerStorage();
    RandForOramInterface* random = new RandomForOram();
    rORAM oram(storage, random, bucketSize, numBlocks, maxRangeSize);

    cout << "Warming up the stash by writing blocks" << endl;

    // Write blocks to rORAM
    for (int i = 0; i < 30000; i++) {
        oram.WriteBlock(i % numBlocks, this->sampleData(i));
    }

    // Update blocks in rORAM
    for (int i = 0; i < 5 * numBlocks; i++) {
        oram.WriteBlock(i % numBlocks, this->sampleData(i));
        if (i % 1000 == 0) {
            cout << "Writing Block " << i << " to rORAM." << std::endl;
        }
    }

    // Read blocks from rORAM and verify correctness
    for (int i = 0; i < numBlocks; i++) {
        std::vector<Block> blocks = oram.ReadRange(i, i + 1); // Read a single block
        if (blocks.empty()) {
            cout << "Reading Block " << i << " from rORAM. Block not found!" << std::endl;
        } else {
            string holder = "";
            int* correct_data = sampleData(4 * numBlocks + i);
            for (unsigned int j = 0; j < Block::BLOCK_SIZE; ++j) {
                int temp = blocks[0].getData()[j];
                if (correct_data[j] != temp) {
                    cout << "Expected and found values don't match!" << endl;
                }
                holder += to_string(temp);
                holder += " ";
            }
            if (i % 1000 == 0) {
                cout << "Reading Block " << i << " from rORAM. Value is : " << holder << std::endl;
            }
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
