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
        for i in range(2 ** (self.height) - 1):
            random_path = random.randint(0, 2 ** (self.height) - 1)
            self.positionMap[i] = (random_path, 0)

        self.key = Fernet.generate_key()
        self.fernet = Fernet(self.key)

        self.server = None
    
    def printPositions(self):
        for key in self.positionMap:
            print(f"key: {key} | value: {self.positionMap[key]}")
    
    def printStash(self):
        print(f"----------------------beginning of stash of size {len(self.stash)}")
        for block in self.stash:
            print(f"blockId: {block.getKey()} | BlockData: {block.getData()}")
        
        print("----------------------end of stash")
    
    def access(self, op, a, newdata = None):
        a_path_num = self.positionMap[a][0]
        firstTime = self.positionMap[a][1]
        randompath = random.randint(0, 2 ** (self.height) - 1)
        self.positionMap[a] = (randompath, 1)
        
        for i in range(self.height):
            self.stash += (self.readBucket(self.server.getBucket(a_path_num, i)))
        
        data = None
        if firstTime == 0:
            data = 0
            for block in self.stash:
                if self.isDummy(block):
                    block.updateKey(a)
                    if op == "w":
                        block.updateData(newdata)
                    else:
                        block.updateData(data)
            
        else:
            for block in self.stash:
                if block.getKey() == a:
                    data = block.data
                    if op == "w":
                        block.updateData(newdata)
        
        #self.printStash()
        
        for i in range(self.height - 1, -1, -1):
            temp_stash = []
            for block in self.stash:
                if not self.isDummy(block):
                    b_block_path = self.positionMap[block.getKey()][0]
                    if self.server.getBucket(b_block_path, i) == self.server.getBucket(a_path_num, i):
                        temp_stash.append(block)
            
            while (len(temp_stash) > self.bucketSize):
                temp_stash.pop()
            
            for block in temp_stash:
                if block in self.stash:
                    self.stash.remove(block)
                
            while len(temp_stash) < self.bucketSize:
                temp_stash.append(self.createDummyBlock())

            newEncryptedBucket = self.encrypt(temp_stash)
            self.server.writeBucket(a_path_num, i, newEncryptedBucket)
        
        return data
    
    def readBucket(self, bucket):
        bucketCopy = bucket.copy()
        listOfBlocks = bucketCopy.getBucket()
        for block in listOfBlocks:
            encKey = block.getKey()
            encData = block.getData()

            decKey = int(self.fernet.decrypt(encKey).decode())
            decData = int(self.fernet.decrypt(encData).decode())

            block.updateKey(decKey)
            block.updateData(decData)
        
        return listOfBlocks
    
    # initialization ---------------------------------------------------------------
    def encryptBlock(self, block):
        strKey = str(block.getKey())
        strData = str(block.getData())

        encKey = self.fernet.encrypt(strKey.encode())
        encData = self.fernet.encrypt(strData.encode())

        block.updateKey(encKey)
        block.updateData(encData)
        return block

    def encrypt(self, listofblocks):
        returnlist = []
        for block in listofblocks:
            returnlist.append(self.encryptBlock(block))

        return Bucket(returnlist)   
    def connectServer(self, server):
        self.server = server

    def initializeTree(self):
        if self.server == None:
            return
        
        totalNodes = 2 ** (self.height + 1) - 1
        for i in range(totalNodes):
            dummyBucket = self.createBucket()
            self.server.insert(dummyBucket)
    
    def createBucket(self):
        returnbucket = []
        for i in range(self.bucketSize):
            returnbucket.append(self.encryptBlock(self.createDummyBlock()))
        return Bucket(returnbucket)
    
    def createDummyBlock(self):
        return self.createBlock()
    
    def createBlock(self, key = -1, data = -1):
        newBlock = Block(key, data)
        return newBlock

    def isDummy(self, block):
        if block.getKey() == -1 and block.getData() == -1:
            return True
        
        return False

numBlocks = 10
bucketSize = 4

height = int(math.log2(numBlocks)) + 1

localClient = Client(numBlocks, bucketSize)

server = Server(height, bucketSize)
localClient.connectServer(server)

localClient.initializeTree()

while(1):
    op = "None"
    print("------------------------------------------------------")
    # localClient.printPositions()
    while(op != "w" and op != "r" and op != "q"):
        op = input("Read (r) or Write (w) or Quit (q): ")
    
    if op == "q":
        break

    userBlock = -1
    while(userBlock < 0 or userBlock > numBlocks):
        try:
            userBlock = int(input(f"Which block do you want to access (0 - {numBlocks - 1}): "))
            if (userBlock < 0 or userBlock > numBlocks):
                print(f"Choose a block inside the range (0 - {numBlocks - 1})")
        except ValueError:
            print("Invalid Integer")

    if op == "w":
        userNewData = None
        while(userNewData == None):
            try:
                userNewData = int(input(f"New int Data for {userBlock}: "))
            except ValueError:
                print("invalid Integer")
        localClient.access(op, userBlock, userNewData)
    
    if op == "r":
        print(localClient.access(op, userBlock))
