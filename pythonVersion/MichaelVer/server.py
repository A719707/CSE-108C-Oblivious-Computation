from tree import Tree
import math

class Server():
    def __init__(self, height, bucketSize, root):
        self.tree = Tree(height, bucketSize)
        self.tree.setRoot(root)

    '''
    input:  int x which is the path number
            int l which height of the path
    output: returns a copy of the node at that position
    '''
    def getBucket(self, x, l):
        return (self.tree.getNode(x, l)).getNodeBucket()

    def printTree(self):
        self.tree.printTree()

    '''
    input:  int path_num is the desired path
    output: returns 
    '''
    def getPath(self, path_num):
        return self.tree.getPath(path_num)

if __name__ == "__main__":
    testServer = Server(8, 4)
    testServer.getBucket(3, 2)
    testServer.getPath(3)