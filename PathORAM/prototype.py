import random
import math
from cryptography.fernet import Fernet

from server import Server
from block import Block
from bucket import Bucket

class Client:
    def __init__(self, numBlocks, bucketSize):
        self.numBlocks = numBlocks
        self.bucketSize = bucketSize
        self.stash = []
        self.height = int(math.log2(numBlocks)) + 1
        self.positionMap = {}
        self.subORAMs = []

        for i in range(self.height):
            random_path = random.randint(0, 2 ** (self.height) - 1)
            self.positionMap[i] = (random_path, 0)

        self.key = Fernet.generate_key()
        self.fernet = Fernet(self.key)
        self.server = Server(self.height, bucketSize)
    
    def readRange(self, oramIndex, a):
        result = []
        U = range(a, a + 2 ** oramIndex)
        
        for block in self.stash:
            if block.getKey() in U:
                result.append(block)
        
        path = self.positionMap[a][0]
        newPath = random.randint(0, 2 ** (self.height) - 1)
        self.positionMap[a] = (newPath, 1)
        
        for level in range(self.height):
            bucket = self.server.getBucket(path, level)
            for block in bucket.getBucket():
                if block.getKey() in U and block not in result:
                    result.append(block)
        
        return result, newPath
    
    def batchEvict(self, oramIndex, k):
        for level in range(self.height):
            for _ in range(k):
                bucket = self.server.getBucket(random.randint(0, 2 ** (self.height) - 1), level)
                temp_stash = []
                for block in self.stash:
                    if block.getKey() != -1:
                        temp_stash.append(block)
                
                while len(temp_stash) > self.bucketSize:
                    temp_stash.pop()
                
                while len(temp_stash) < self.bucketSize:
                    temp_stash.append(self.createDummyBlock())

                bucket.updateBucket(temp_stash)
    
    def access(self, op, a, r, newData=None):
        oramIndex = int(math.log2(r))
        rangeSize = 2 ** oramIndex
        
        a1 = (a // rangeSize) * rangeSize
        a2 = (a1 + rangeSize) % self.numBlocks
        
        data1, p1 = self.readRange(oramIndex, a1)
        data2, p2 = self.readRange(oramIndex, a2)

        if op == 'r':
            return data1 + data2

        if op == 'w':
            for i, block in enumerate(data1 + data2):
                if i < len(newData):
                    block.updateData(newData[i])

        self.stash.extend(data1 + data2)
        
        for i in range(self.height):
            self.batchEvict(i, rangeSize)

    def createDummyBlock(self):
        return Block(-1, -1)

if __name__ == "__main__":
    numBlocks = 2**7
    bucketSize = 4
    client = Client(numBlocks, bucketSize)
    
    while True:
        op = input("Read (r) or Write (w) or Quit (q): ")
        if op == 'q':
            break

        try:
            blockStart = int(input(f"Enter the starting block (0-{numBlocks-1}): "))
            rangeSize = int(input("Enter range size (power of 2): "))
            if op == 'w':
                data = [int(input(f"Data for block {i}: ")) for i in range(blockStart, blockStart + rangeSize)]
                client.access(op, blockStart, rangeSize, data)
            elif op == 'r':
                print(client.access(op, blockStart, rangeSize))
        except ValueError:
            print("Invalid input. Try again.")