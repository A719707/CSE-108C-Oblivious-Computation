
class Node: 
    def __init__(self, num):
        self.num = num

        self.left = None
        self.right = None
        self.bucket = None
        self.path_num = -1

    def updateBucket(self, bucket):
        self.bucket = bucket

    def getNodeBucket(self):
        if self.bucket != None:
            return self.bucket
        
        else:
            raise Exception("Node.getNodeBucket: bucket does not exist")
    
    def copy(self):
        newNode = Node(self.num)
        newNode.left = self.left
        newNode.right = self.right
        newNode.bucket = self.bucket.copy()
        newNode.path_num = self.path_num

        return newNode