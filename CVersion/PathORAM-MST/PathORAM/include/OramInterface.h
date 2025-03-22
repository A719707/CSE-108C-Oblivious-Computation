#ifndef ORAM_INTERFACE_H
#define ORAM_INTERFACE_H

#include "Block.h"
#include <vector>

class OramInterface {
public:
    enum Operation { READ, WRITE };

    virtual int* access(Operation op, int blockIndex, int *newdata) = 0;
    virtual std::vector<Block> ReadRange(int start, int end) = 0; // Add this line
    virtual ~OramInterface() {}
};

#endif // ORAM_INTERFACE_H
