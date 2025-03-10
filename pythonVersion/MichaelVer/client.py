import random
import server

class Client():
    def __init__(self):
        self.position_map = None
        self.stash = None
    
    def updatePositionMap(self, index, newPath):
        self.position_map[index] = newPath

    def getPosition(self, a):
        return self.position_map[a]

    def access(self, op, a, data):
        path = self.getPosition(a)
        height = 4
        self.updatePositionMap(a, random.randint(2**(height) - 1))