
#include "CorrectnessTester1.h"
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

CorrectnessTester1::CorrectnessTester1() {
}

int* CorrectnessTester1::sampleData(int i) {
    int* newArray = new int[Block::BLOCK_SIZE];
    for (int j = 0; j < Block::BLOCK_SIZE; ++j) {
        newArray[j] = i;
    }
    return newArray;
}

void CorrectnessTester1::runCorrectnessTest() {
    int bucketSize = 4;
    int numBlocks = pow(2, 12);
    Bucket::setMaxSize(bucketSize);

    UntrustedStorageInterface* storage = new ServerStorage();
    RandForOramInterface* random = new RandomForOram();
    OramInterface* oram = new OramReadPathEviction(storage, random, bucketSize, numBlocks);

    cout << "Warming up the stash by writing blocks" << endl;

    for (int i = 0; i < 3000; i++) {
        int* accessed = oram->access(OramInterface::Operation::WRITE, i % numBlocks, this->sampleData(i));
        if (oram->getStashSize() != 0 && i % 10 == 0) {
            cout << "Writing Block " << i % numBlocks << " to ORAM. The stash size is: " << to_string(oram->getStashSize()) << std::endl;
        }
    }

    for (int i = 0; i < numBlocks; i++) {
        int* accessed = oram->access(OramInterface::Operation::READ, i, NULL);
        string holder = "";
        if (accessed == NULL) {
            cout << "Reading Block " << i << " from ORAM. Block not found!" << std::endl;
        } else {
            for (unsigned int j = 0; j < Block::BLOCK_SIZE; ++j) {
                int temp = accessed[j];
                holder += to_string(temp);
                holder += " ";
            }
            if (i % 1000 == 0) {
                cout << "Reading Block " << i << " from ORAM. Value is : " << holder << std::endl;
                cout << "Stash size is " << to_string(oram->getStashSize()) << std::endl;
            }
        }
    }

    delete oram;
    delete random;
    delete storage;
}

void CorrectnessTester1::runRORAMCorrectnessTest() {
    int bucketSize = 4;
    int numBlocks = pow(2, 12);
    int maxRangeSize = 16; // Maximum range size supported by rORAM
    Bucket::setMaxSize(bucketSize);

    UntrustedStorageInterface* storage = new ServerStorage();
    RandForOramInterface* random = new RandomForOram();
    rORAM oram(storage, random, bucketSize, numBlocks, maxRangeSize);

    cout << "Warming up the stash by writing blocks" << endl;

    // Write blocks to rORAM
    for (int i = 0; i < 3000; i++) {
        oram.WriteBlock(i % numBlocks, this->sampleData(i));
        if (i % 10 == 0) {
            cout << "Writing Block " << i % numBlocks << " to rORAM." << std::endl;
        }
    }

    // Read blocks from rORAM
    for (int i = 0; i < numBlocks; i++) {
        std::vector<Block> blocks = oram.ReadRange(i, i + 1); // Read a single block
        if (blocks.empty()) {
            cout << "Reading Block " << i << " from rORAM. Block not found!" << std::endl;
        } else {
            string holder = "";
            for (unsigned int j = 0; j < Block::BLOCK_SIZE; ++j) {
                int temp = blocks[0].getData()[j];
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
