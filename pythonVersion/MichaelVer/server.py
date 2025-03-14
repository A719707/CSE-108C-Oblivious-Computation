from tree import Tree

class Server():
    def __init__(self):
        self.tree = None
        self.bucket_size = None
        self.height = None
    
    def makeTree(self, bucketsize, height):
        new_tree = Tree(bucketsize)
        self.tree = new_tree.createTree(height)
        self.height = height

    def getBucket(self, x, l):
        return self.tree.getNode(x, l).bucket

    def getPath(self, path_num):
        for height in range(self.server_height):
            bucket = self.server.getBucket(path_num, height)
            self.stash.append(bucket)