from client import Client
from server import Server

import math
import random

import time

for j in range(1, 10):
    numBlocks = 2**j
    bucketSize = 4

    height = int(math.log2(numBlocks)) + 1

    localClient = Client(numBlocks, bucketSize)

    server = Server(height, bucketSize)
    localClient.connectServer(server)

    localClient.initializeTree()

    choices = ["r", "w"]

    readtime = 0
    writetime = 0

    iterations = 1000
    progress = 0
    percentCheck = 0.25
    for i in range(iterations):
        if i%(iterations * percentCheck) == 0:
            print(f"{progress}%")
            progress+= (percentCheck * 100)
        op = random.choice(choices)
        randomBlock = random.randint(0, numBlocks)
        if op == "w":
            randomData = random.randint(0, numBlocks)
            start_time = time.time()
            localClient.access(op, randomBlock, randomData)
            end_time = time.time()
            writetime += (end_time- start_time)
        else:
            start_time = time.time()
            localClient.access(op, randomBlock)
            end_time = time.time()
            readtime += (end_time- start_time)

    print(f"the average time for read of size {numBlocks}: {readtime/iterations}")
    print(f"the average time for write of size {numBlocks}: {writetime/iterations}")

