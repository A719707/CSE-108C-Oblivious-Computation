#include "OramReadPathEviction.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>

using namespace std;

OramReadPathEviction::OramReadPathEviction(UntrustedStorageInterface* storage, RandForOramInterface* rand_gen,
                                           int bucket_size, int num_blocks)
    : storage(storage), rand_gen(rand_gen), bucket_size(bucket_size), num_blocks(num_blocks) {

    this->num_levels = ceil(log2(num_blocks)) + 1;
    this->num_buckets = (1 << num_levels) - 1;

    if (this->num_buckets * this->bucket_size < this->num_blocks) {
        throw runtime_error("Not enough space for the actual number of blocks.");
    }

    this->num_leaves = 1 << (num_levels - 1);
    this->rand_gen->setBound(num_leaves);
    this->storage->setCapacity(num_buckets);
    this->position_map.resize(num_blocks);

    for (int i = 0; i < num_blocks; i++) {
        position_map[i] = rand_gen->getRandomLeaf();
    }

    for (int i = 0; i < num_buckets; i++) {
        Bucket init_bkt;
        for (int j = 0; j < bucket_size; j++) {
            init_bkt.addBlock(Block());
        }
        storage->WriteBucket(i, init_bkt);
    }
}

vector<int> OramReadPathEviction::access(Operation op, int blockIndex, vector<int> newdata) {
    vector<int> data(Block::BLOCK_SIZE, 0);
    int oldLeaf = position_map[blockIndex];
    position_map[blockIndex] = rand_gen->getRandomLeaf();

    for (int i = 0; i < num_levels; i++) {
        vector<Block> blocks = storage->ReadBucket(P(oldLeaf, i)).getBlocks();
        for (Block &b : blocks) {
            if (b.index != -1) {
                stash.push_back(b);
            }
        }
    }

    Block *targetBlock = nullptr;
    for (auto &b : stash) {
        if (b.index == blockIndex) {
            targetBlock = &b;
            break;
        }
    }

    if (op == Operation::WRITE) {
        if (targetBlock == nullptr) {
            stash.emplace_back(position_map[blockIndex], blockIndex, newdata);
        } else {
            targetBlock->data = newdata;
        }
    } else {
        if (targetBlock != nullptr) {
            data = targetBlock->data;
        }
    }

    for (int l = num_levels - 1; l >= 0; l--) {
        vector<int> bid_evicted;
        Bucket bucket;
        int Pxl = P(oldLeaf, l);

        for (Block &b_instash : stash) {
            if (bucket.getSize() >= bucket_size) break;
            if (Pxl == P(position_map[b_instash.index], l)) {
                bucket.addBlock(b_instash);
                bid_evicted.push_back(b_instash.index);
            }
        }

        stash.erase(remove_if(stash.begin(), stash.end(),
                              [&](Block &b) { return find(bid_evicted.begin(), bid_evicted.end(), b.index) != bid_evicted.end(); }),
                    stash.end());

        while (bucket.getSize() < bucket_size) {
            bucket.addBlock(Block());
        }
        storage->WriteBucket(Pxl, bucket);
    }

    return data;
}

