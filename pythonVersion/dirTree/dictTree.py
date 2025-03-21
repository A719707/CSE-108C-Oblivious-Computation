class Tree:
    def __init__(self, height, bucketsize):
        self.dictNodes = {}
        self.bucketsize = bucketsize
        self.treeheight = height
        self.maxLeaves = 2 ** height
    
    def insert(self, bucket):
        if len(self.dictNodes) < (2**(self.treeheight + 1) - 1):
            nextNum = len(self.dictNodes)
            self.dictNodes[nextNum] = bucket
        
        return None

    def getParentNodeNum(self, currentNodeNum):
        return (currentNodeNum - 1)//2
    
    def getNumPathHeight(self, pathnum, height):
        desiredNodeNum = desiredNodeNum = 2 ** self.treeheight + pathnum - 1
        for i in range(self.treeheight - height):
            desiredNodeNum = self.getParentNodeNum(desiredNodeNum)
        return desiredNodeNum
    
    def getBucket(self, pathnum, height):
        returnbucket = self.dictNodes[self.getNumPathHeight(pathnum, height)]
        return returnbucket

    def updateBucket(self, pathnum, height, newbucket):
        self.dictNodes[self.getNumPathHeight(pathnum, height)] = newbucket