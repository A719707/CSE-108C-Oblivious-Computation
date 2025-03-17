class Block:
    def __init__(self, key = 0, data = 0):
        self.key = key
        self.data = data
    
    # accessor functions -------------------------------------------------------------------------
    def getKey(self):
        return self.key
    
    def getData(self):
        return self.data
    
    # updator functions --------------------------------------------------------------------------
    def updateKey(self, newKey):
        self.key= newKey
    
    def updateData(self, newData):
        self.data = newData

    # --------------------------------------------------------------------------------------------
    def copy(self):
        newBlock = Block(self.getKey(), self.getData())
        return newBlock


    # print functions ----------------------------------------------------------------------------
    def printBlock(self):
        print("key: " + str(self.getKey()))
        print("Data: " + str(self.getData()))
        return
