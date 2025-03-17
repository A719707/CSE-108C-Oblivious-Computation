
class Bucket:
    def __init__(self, bucketsize, listOfBlocks):
        self.bucket = listOfBlocks
        self.bucketsize = bucketsize
        self.numrRealNodes = 0
    
    def replaceBucket(self, newBucket):
        self.bucket = newBucket
    
    def getListBlocks(self):
        return self.bucket

    def printBucket(self):
        print("------------------------------------------ new Bucket")
        for block_in_bucket in self.bucket:
            block_in_bucket.printBlock()
            print("------------------------------------------")

        return
    
    def copy(self):
        newBucket = Bucket(self.bucketsize)
        for i in range(len(self.bucket)):
            newBucket.put(self.bucket[i])

        return newBucket
            