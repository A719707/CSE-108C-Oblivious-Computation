from dictTree import Tree

class Server:
    def __init__(self, height, bucketsize):
        self.tree = Tree(height, bucketsize)

    def insert(self, bucket):
        self.tree.insert(bucket)
    
    def getBucket(self, pathnum, height):
        returnbucket = self.tree.getBucket(pathnum, height)
        if returnbucket == None:
            raise Exception("Server.getBucket: getBucket was NONE!")
        return returnbucket
    
    def writeBucket(self, pathnum, height, newBucket):
        self.tree.updateBucket(pathnum, height, newBucket)