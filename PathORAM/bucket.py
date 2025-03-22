class Bucket:
    def __init__(self, listofBlocks = []):
        self.listOfBlocks = listofBlocks
    def getBucket(self):
        return self.listOfBlocks
    def updateBucket(self, newBucket):
        self.listOfBlocks = newBucket
    def copy(self):
        newBucket = []
        for block in self.listOfBlocks:
            newBucket.append(block.copy())
        return Bucket(newBucket)