class Block:
    def __init__(self):
        self.key = 0
        self.data = 69420
    
    def getKey(self):
        return self.key
    
    def getData(self):
        return self.data
    
    def printBlock(self):
        print("key: " + str(self.getKey()))
        print("Data: " + str(self.getData()))
        return
