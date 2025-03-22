
#include "CorrectnessTester4.h"
#include "Bucket.h"
#include "ServerStorage.h"
#include "OramInterface.h"
#include "RandForOramInterface.h"
#include "RandomForOram.h"
#include "UntrustedStorageInterface.h"
#include "OramReadPathEviction.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

CorrectnessTester4::CorrectnessTester4() {
    // Constructor (if needed)
}

int* CorrectnessTester4::sampleData(int i) {
    int* newArray = new int[Block::BLOCK_SIZE];
    for (int j = 0; j < Block::BLOCK_SIZE; ++j) {
        newArray[j] = i; // Fill the block with the value `i`
    }
    return newArray;
}

void CorrectnessTester4::runCorrectnessTest() {
    // Configuration parameters
    int bucketSize = 10;
    int numBlocks = pow(2, 12); // 4096 blocks
    Bucket::setMaxSize(bucketSize);

    // Initialize storage and randomness
    UntrustedStorageInterface* storage = new ServerStorage();
    RandForOramInterface* random = new RandomForOram();

    // Initialize ORAM
    OramInterface* oram = new OramReadPathEviction(storage, random, bucketSize, numBlocks);

    cout << "Warming up the stash by writing blocks..." << endl;

    // Warm-up phase: Write blocks to ORAM
    for (int i = 0; i < 30000; i++) {
        int blockIndex = i % numBlocks; // Cycle through block indices
        int* data = sampleData(i); // Generate sample data
        int* accessed = oram->access(OramInterface::Operation::WRITE, blockIndex, data);
        delete[] data; // Clean up dynamically allocated data
    }

    // Main phase: Perform additional writes and reads
    for (int i = 0; i < 5 * numBlocks; i++) {
        int blockIndex = i % numBlocks;
        int* data = sampleData(i);
        int* accessed = oram->access(OramInterface::Operation::WRITE, blockIndex, data);
        delete[] data;
    }

    // Verification phase: Read blocks and verify correctness
    for (int i = 0; i < numBlocks; i++) {
        int* accessed = oram->access(OramInterface::Operation::READ, i, nullptr);
        string holder = "";
        int* correctData = sampleData(4 * numBlocks + i); // Expected data

        // Verify each element in the block
        for (unsigned int j = 0; j < Block::BLOCK_SIZE; ++j) {
            if (correctData[j] != accessed[j]) {
                cout << "Error: Expected and found values don't match for block " << i << " at position " << j << "!" << endl;
                cout << "Expected: " << correctData[j] << ", Found: " << accessed[j] << endl;
            }
            holder += to_string(accessed[j]) + " ";
        }

        // Log progress periodically
        if (i % 1000 == 0) {
            cout << "Reading Block " << i << " from ORAM. Value is: " << holder << endl;
            cout << "Stash size is " << oram->getStashSize() << endl;
        }

        delete[] correctData; // Clean up dynamically allocated data
    }

    // Clean up
    delete oram;
    delete random;
    delete storage;
}

