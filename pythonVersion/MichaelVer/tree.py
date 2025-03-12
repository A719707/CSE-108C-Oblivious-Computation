from bucket import Bucket

class Node: 
    def __init__(self, num, bucketsize):
        self.num = num
        self.left = None
        self.right = None
        self.bucket = Bucket(bucketsize)

        self.path_num = -1

class Tree:
    def __init__(self, height = None, bucketsize = 4):
        self.root = None
        self.bucketsize = bucketsize
        self.height = height

        if height != None:
            if height >= 0:
                self.createTree(height)
    
    #Accessors ----------------------------------------------------------------------------
    def getBucketsize(self):
        return self.bucketsize

    def getHeight(self):
        return self.height

    #Initializers -------------------------------------------------------------------------
    def createTree(self, depth):
        if depth == 0:
            return Node(0, self.getBucketsize())
        queue = []
        self.height = depth
        i = 0
        leaf_num = 0
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

                    if i == depth:
                        node.left.path_num = leaf_num
                        leaf_num += 1

                        node.right.path_num = leaf_num
                        leaf_num += 1

                    queue.append(node.left)
                    queue.append(node.right)
        return root

    # Special Functions --------------------------------------------------------------------------------------
    def getBucket(self, x, l):
        path = self.getPath(x)
        for node in path:
            if node.num == l:
                print(node.num, end = "")
                print(node.path_num)
                return node.bucket

    def getPath(self, path_num):
        path = self.dfs(self.root, path_num)
        # for node in path:
        #     print(node.num, end = "")
        #     print(node.path_num)
        return path
    
    def dfs(self, current_node, path_num, path = None):
        num1 = current_node.path_num
        num2 = path_num

        # print(num1, end = " ")
        # print(num2, end = " ")
        # print(num1 == num2)

        if path == None:
            path = []

        path.append(current_node)
        if num1 == num2:
            return path
        
        if current_node.left == None:
            return None

        if (current_node.left not in path):
            result = self.dfs(current_node.left, path_num, path.copy())
            if result:
                return result
        
        if current_node.right not in path:
            result = self.dfs(current_node.right, path_num, path.copy())
            if result:
                return result
                
        return None

    # printing --------------------------------------------------------------------------------------------

    def inorder(self, root, row, col, height, ans):
        if not root:
            return
        
        offset = 2 ** (height - row - 1)

        if root.left:
            self.inorder(root.left, row + 1, col - offset, 
                    height, ans)

        ans[row][col] = str(root.num) + str(root.path_num)   #this changes what gets print

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
    print("Testing tree.py file")
    height = int(input("Tree height: "))
    a = Tree(height)
    a.printTree()
    a.getPath(1)
    a.getBucket(7, 3)

