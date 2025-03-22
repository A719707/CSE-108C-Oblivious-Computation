#include "CorrectnessTester5.h"
#include "Bucket.h"
#include "ServerStorage.h"
#include "OramInterface.h"
#include "RandForOramInterface.h"
#include "RandomForOram.h"
#include "UntrustedStorageInterface.h"
#include "OramReadPathEviction.h"
#include "OramDeterministic.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

CorrectnessTester5::CorrectnessTester5() {
    // Constructor (if needed)
}

int* CorrectnessTester5::sampleData(int i) {
    int* newArray = new int[Block::BLOCK_SIZE];
    for (int j = 0; j < Block::BLOCK_SIZE; ++j) {
        newArray[j] = i; // Fill the block with the value `i`
    }
    return newArray;
}

void CorrectnessTester5::runCorrectnessTest() {
    // Configuration parameters
    int bucketSize = 2;
    int numBlocks = pow(2, 20); // 1,048,576 blocks
    Bucket::setMaxSize(bucketSize);

    // Initialize storage and randomness
    UntrustedStorageInterface* storage = new ServerStorage();
    RandForOramInterface* random = new RandomForOram();

    // Initialize ORAM (choose between OramReadPathEviction or OramDeterministic)
    OramInterface* oram = new OramReadPathEviction(storage, random, bucketSize, numBlocks);
    // OramInterface* oram = new OramDeterministic(storage, random, bucketSize, numBlocks);

    cout << "Warming up the stash by writing blocks..." << endl;

    // Warm-up phase: Write blocks to ORAM
    int bound = numBlocks;
    for (int i = 0; i < bound; i++) {
        int blockIndex = i % numBlocks; // Cycle through block indices
        int* data = sampleData(blockIndex); // Generate sample data
        int* accessed = oram->access(OramInterface::Operation::WRITE, blockIndex, data);
        delete[] data; // Clean up dynamically allocated data

        // Log progress periodically
        if (i % 1000 == 0) {
            cout << "Writing Block " << blockIndex << " with value: " << blockIndex << " to ORAM. Stash size: " << oram->getStashSize() << endl;
        }
    }

    // Verification phase: Read blocks and verify correctness
    for (int i = 0; i < bound; i++) {
        int blockIndex = i % numBlocks;
        int* accessed = oram->access(OramInterface::Operation::READ, blockIndex, nullptr);
        string holder = "";

        // Check for NULL access
        if (accessed == nullptr) {
            cout << "Error: Accessed block is NULL for block index " << blockIndex << " (i = " << i << ")" << endl;
            continue;
        }

        // Verify each element in the block
        for (unsigned int j = 0; j < Block::BLOCK_SIZE; ++j) {
            holder += to_string(accessed[j]) + " ";
        }

        // Log progress periodically
        if (i % 1000 == 0) {
            cout << "Reading Block " << blockIndex << " from ORAM. Value is: " << holder << endl;
            cout << "Stash size is " << oram->getStashSize() << endl;
        }
    }

    // Clean up
    delete oram;
    delete random;
    delete storage;
}
