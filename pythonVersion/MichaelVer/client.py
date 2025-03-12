import random
import server

class Client():
    def __init__(self, server_height):
        temp_dict = {}
        num_leaves  = 2**(server_height)
        for i in range(num_leaves):
            random_path = random.randint(0, num_leaves - 1)
            temp_dict[i] = random_path
        
        self.position_map = temp_dict
        self.stash = []
        self.server_height = server_height
        self.max_leaves = num_leaves
    
    def _updatePositionMap(self, a):
        new_path = random.randint(0, self.max_leaves - 1)
        self.position_map[a] = new_path

    def _getPosition(self, a):
        if a in self.position_map:  
            return self.position_map[a]
        return False

    def access(self, op, a, data):
        path_a = self._getPosition(a)
        self._updatePositionMap(path_a)

        for height in range(self.server_height):
            bucket = 
            self.stash.append()

