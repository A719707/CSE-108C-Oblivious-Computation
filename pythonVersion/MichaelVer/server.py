from tree import Tree

class Server():
    def __init__(self):
        self.tree = None
        self.bucket_size = None
    
    def makeTree(self, bucketsize, height):
        new_tree = Tree(bucketsize)
        self.tree = new_tree.createTree(height)

