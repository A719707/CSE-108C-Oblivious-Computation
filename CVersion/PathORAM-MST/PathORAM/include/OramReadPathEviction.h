#ifndef ORAM_READ_PATH_EVICTION_H
#define ORAM_READ_PATH_EVICTION_H

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

// Interface for untrusted storage
class UntrustedStorageInterface {
public:
    virtual ~UntrustedStorageInterface() = default;
    virtual void write(int address, const vector<int>& data) = 0;
    virtual vector<int> read(int address) = 0;
};

// Concrete implementation of untrusted storage
class UntrustedStorage : public UntrustedStorageInterface {
public:
    void write(int address, const vector<int>& data) override {
        // Implementation of write to storage
        cout << "Writing to address " << address << ": ";
        for (int val : data) {
            cout << val << " ";
        }
        cout << endl;
    }

    vector<int> read(int address) override {
        // Implementation of read from storage
        cout << "Reading from address " << address << endl;
        return {10, 20}; // Dummy data for demonstration
    }
};

// Interface for random number generation
class RandForOramInterface {
public:
    virtual ~RandForOramInterface() = default;
    virtual int getRandomLeaf() = 0;
};

// Concrete implementation of random number generation
class RandForOram : public RandForOramInterface {
public:
    int getRandomLeaf() override {
        // Implementation of random leaf generation
        return rand() % 1024; // Dummy random leaf for demonstration
    }
};

// ORAM class implementing read-path eviction
class OramReadPathEviction {
public:
    enum Operation { READ, WRITE };

    OramReadPathEviction(UntrustedStorageInterface* storage, RandForOramInterface* rand_gen, int bucketSize, int numBlocks);
    ~OramReadPathEviction();

    int* access(Operation op, int blockId, int* newData);

private:
    UntrustedStorageInterface* storage;
    RandForOramInterface* rand_gen;
    int bucketSize;
    int numBlocks;

    // Internal methods for ORAM operations
    void writeBlock(int blockId, const vector<int>& data);
    vector<int> readBlock(int blockId);
    void evictPath(int leaf);
};

#endif // ORAM_READ_PATH_EVICTION_H
