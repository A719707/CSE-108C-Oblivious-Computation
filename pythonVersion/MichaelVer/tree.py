from bucket import Bucket

class Node: 
    def __init__(self, num, bucketsize):
        self.num = num
        self.left = None
        self.right = None
        self.bucket = Bucket(bucketsize)

class Tree:
    def __init__(self, bucketsize = 4):
        self.root = None
        self.bucketsize = bucketsize
        self.height = None
    
    def getBucketsize(self):
        return self.bucketsize

    def createTree(self, depth):
        if depth == 0:
            return Node(0, self.getBucketsize())
        queue = []
        self.height = depth
        i = 0
        self.root = Node(i, self.getBucketsize())
        root = self.root

        queue.append(root)
        while len(queue) > 0:
            size = len(queue)
            i += 1
            if i > depth:
                break
            else:
                for j in range(size):
                    node = queue.pop(0)
                    node.left = Node(i, self.getBucketsize())
                    node.right = Node(i, self.getBucketsize())

                    queue.append(node.left)
                    queue.append(node.right)
        return root

    def getPath(self, path):
        return

    def inorder(self, root, row, col, height, ans):
        if not root:
            return
        
        offset = 2 ** (height - row - 1)

        if root.left:
            self.inorder(root.left, row + 1, col - offset, 
                    height, ans)

        ans[row][col] = str(root.num)

        if root.right:
            self.inorder(root.right, row + 1, col + offset, 
                    height, ans)

    def tree_to_matrix(self, root):
        height = self.height

        rows = height + 1
        cols = 2 ** (height + 1) - 1

        ans = [["" for _ in range(cols)] for _ in range(rows)]

        self.inorder(root, 0, (cols - 1) // 2, height, ans)

        return ans
    
    def print_2d_array(self, arr):
        for row in arr:
            for cell in row:
                if cell == "":
                    print(" ", end="")
                else:
                    print(cell, end="")
            print()

    def printTree(self):
        result = self.tree_to_matrix(self.root)
        self.print_2d_array(result)

if __name__ == "__main__":
    a = Tree()
    a.perfectBinaryTree(3)
    a.printTree()

