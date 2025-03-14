from block import Block

class Bucket:
    def __init__(self, bucketsize):
        self.bucket = [Block() for i in range(bucketsize)]
    
    '''
    input: int key
    '''
    def pop(self, key):
        return_block = None
        i = 0
        for block_in_bucket in self.bucket:
            if key == block_in_bucket.getKey():
                return_block = block_in_bucket
                self.bucket[i] = Block()
                #break block
            i+=1
        
        #do we need to search through every block for obliviousness?
        if return_block == None:
            return 0
        
        else:
            return return_block

    def printBucket(self):
        print("------------------------------------------ new Bucket")
        for block_in_bucket in self.bucket:
            block_in_bucket.printBlock()
            print("------------------------------------------")

        return
    
    def readBucket(self, searchkey):
        list_of_blocks = self.bucket
        for block in list_of_blocks:
            if block.key == searchkey:
                return block
        
        return "Not here"
            