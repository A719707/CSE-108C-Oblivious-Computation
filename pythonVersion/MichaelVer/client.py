import random
import math
from cryptography.fernet import Fernet

from server import Server
from block import Block
from bucket import Bucket
from node import Node

from tree import Tree

class Client():
    def __init__(self, numBlocks, bucketSize):
        
        self.numBlocks = numBlocks
        self.bucketSize = bucketSize

        temp_dict = {}
        for i in range(numBlocks):
            random_path = random.randint(0, numBlocks - 1)
            temp_dict[i] = random_path
        
        self.position_map = temp_dict

        self.stash = []

        self.key = Fernet.generate_key()
        self.fernet = Fernet(self.key)

        self.server = None

    def connectServer(self, server):
        self.server = server
    
    # accessors -------------------------------------------------------------------
    def getNumBlocks(self):
        return self.numBlocks

    # special functions ----------------------------------------------------------------------
    def _updatePositionMap(self, a):
        new_path = random.randint(0, self.max_leaves - 1)
        self.position_map[a] = new_path
        return new_path

    def _getPosition(self, a):
        if a in self.position_map:  
            return self.position_map[a]
        return False

    # main function ---------------------------------------------------------------

    def access(self, op, a, newdata = None):
        # encMessage = None
        # if newdata:
        #     encMessage = fernet.encrypt(newdata.encode())
            
        path_a_num = self._getPosition(a)
        self._updatePositionMap(path_a_num)

        self.stash = self.server.getPath(path_a_num)

        data = None
        for bucket in self.stash:
            block = self.readBucket(bucket, a)
            if block:
                data = block.getData()
                block.updateData(newdata)
        
        if op == "w": 
            new_stash = []
            new_path_num = self._updatePositionMap(a)
            new_path = self.server.findPath(a)

            

            # self.stash = 

    '''
    input:  bucket = list of Blocks
            int a = block id
    
    output: returns the block containing the block id or None
    '''
    def readBucket(self, bucket, a):
        listOfBlocks = bucket.getListblocks()
        for block in listOfBlocks:
            encKey = block.getkey()
            encData = block.getData()

            decKey = decMessage = self.fernet.decrypt(encKey).decode()
            decData = self.fernet.decrypt(encData).decode()

            if decKey == a:
                return block
            
            newEncKey = self.fernet.encrypt(decKey.encode())
            newEncData = self.fernet.encrypt(decData.encode())

            block.updateKey(newEncKey)
            block.updateData(newEncData)
        
        return None
    
    def writeBucket(self, bucket, a, newData):
        listOfBlocks = bucket.getListblocks()
        strNewKey = str(a)
        strNewData = str(newData)
        for block in listOfBlocks:
            encKey = block.getkey()
            encData = block.getData()

            decKey = decMessage = self.fernet.decrypt(encKey).decode()
            decData = self.fernet.decrypt(encData).decode()

            if decKey == a:
                encNewData = self.fernet.encrypt(strNewData.encode())
                block.updateData(encNewData)
                return block
        
        return None
    
    # Initial creation of tree -----------------------------------------------------------
    def createBlock(self, newKey = 0, newData = 0):

        strNewKey = str(newKey)
        strNewData = str(newData)        

        encKey = self.fernet.encrypt(strNewKey.encode())
        encData = self.fernet.encrypt(strNewData.encode())

        newBlock = Block(encKey, encData)

        return newBlock
    
    def createNewEmptyBucket(self):
        temp_list = []
        for i in range(self.bucketSize):
            temp_list.append(self.createBlock())
        
        return Bucket(self.bucketSize, temp_list)
    
    def newNode(self, node_num):
        newNode = Node(node_num)
        newNode.updateBucket(self.createNewEmptyBucket())

        return newNode
    
    def createTree(self, depth):
        if depth == 0:
            return self.newNode(0)
        queue = []
        i = 0
        leaf_num = 0

        root = self.newNode(i)

        queue.append(root)
        while len(queue) > 0:
            size = len(queue)
            i += 1
            if i > depth:
                break
            else:
                for j in range(size):
                    node = queue.pop(0)
                    node.left = self.newNode(i)
                    node.right = self.newNode(i)

                    if i == depth:
                        node.left.path_num = leaf_num
                        leaf_num += 1

                        node.right.path_num = leaf_num
                        leaf_num += 1

                    queue.append(node.left)
                    queue.append(node.right)
        return root
            
numBlocks = 10
bucketSize = 4

height = int(math.log2(numBlocks)) + 1

localClient = Client(numBlocks, bucketSize)

server = Server(height, bucketSize, localClient.createTree(height))
localClient.connectServer(server)

# while(1):
#     op = "None"
#     while(op != "w" and op != "r" and op != "q"):
#         op = input("Read (r) or Write (w) or Quit (q): ")
    
#     if op == "q":
#         break

#     userBlock = -1
#     while(userBlock < 0 or userBlock > numBlocks):
#         userBlock = int(input(f"Which block do you want to access (0 - {numBlocks}): "))

#     if op == "w":
#         userNewData = None
#         while(userNewData != None):
#             userNewData = int(input(f"New int Data for {userBlock}: "))
#         localClient.access(op, userBlock, userNewData)

    
        

        


