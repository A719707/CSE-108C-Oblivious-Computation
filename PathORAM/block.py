class Block:
    def __init__(self, key, data):
        self.key = key
        self.data = data
    def getKey(self):
        return self.key
    def getData(self):
        return self.data
    def updateKey(self, newkey):
        self.key = newkey
    def updateData(self, newData):
        self.data = newData
    def copy(self):
        newBlock = Block(self.key, self.data)
        return newBlock