## Running PathORAM

to run PathORAM use the command 

python3 client.py

At default the size of the bucket is 4, and the expected number of Blocks is size 2^10. You can change this on the client.py on line 155 and 156

## Files
block.py - contains the block class and its functions

bucket.py - contains the bucket class and its functions

client.py - the main part of PathORAM

dictTree.py - the data structure used by the Server

server.py - contains the tree and basic functions of the tree.
