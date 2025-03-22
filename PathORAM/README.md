## Running PathORAM

to run PathORAM use the command 

python3 client.py

At default the size of the bucket is 4, and the expected number of Blocks is size 2^10. You can change this on the client.py on line 155 and 156

## Running rORAM

to run rORAM use the command 

python3 test_range_oram.py

## Files
block.py - contains the block class and its functions

bucket.py - contains the bucket class and its functions

client.py - the main part of PathORAM

dictTree.py - the data structure used by the Server

server.py - contains the tree and basic functions of the tree.

pathORAMSim.py - tests whether the read and write operations have the same time complexity

pathtester.py - is the unit test for pathORAM, makes sure the functions individually work

prototype.py - was the an attempt on range ORAM in python

range_oram.py - is another version of range oram implemented

test_range_oram.py - is a tester for range_oram.py