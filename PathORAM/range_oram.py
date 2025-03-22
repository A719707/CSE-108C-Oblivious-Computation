import random
import math
from cryptography.fernet import Fernet
from collections import defaultdict
from typing import List, Tuple, Dict, Optional

from server import Server
from block import Block
from bucket import Bucket
from client import Client

class RangeORAM(Client):
    def __init__(self, numBlocks: int, bucketSize: int, range_size: int):
        super().__init__(numBlocks, bucketSize)
        self.range_size = range_size
        self.eviction_rate = int(math.sqrt(numBlocks))  # k
        self.access_counter = 0
        self.range_cache = {}  
        self.blocks = set() 
        
    def ReadRange(self, start_addr: int, end_addr: int) -> List[Tuple[int, int]]:

        if start_addr > end_addr or start_addr < 0 or end_addr >= self.numBlocks:
            raise ValueError("Invalid range")
            
        result = []
        # check cache for any blocks in range
        cached_blocks = {addr: data for addr, data in self.range_cache.items() 
                        if start_addr <= addr <= end_addr}
                        
        # read remaining blocks from ORAM
        for addr in range(start_addr, end_addr + 1):
            if addr in cached_blocks:
                result.append((addr, cached_blocks[addr]))
            else:
                data = self.access("r", addr)
                result.append((addr, data))
                self.range_cache[addr] = data
                
        self.access_counter += 1
        if self.access_counter >= self.eviction_rate:
            self.BatchEvict()
            
        return result
        
    def BatchEvict(self):
        blocks_to_evict = list(self.blocks)[:self.eviction_rate]
        
        for block_addr in blocks_to_evict:
            if block_addr in self.range_cache:
                data = self.range_cache[block_addr]
                self.access("w", block_addr, data)
                del self.range_cache[block_addr]
                self.blocks.remove(block_addr)
        
    def Access(self, addr: int, op: str, data: Optional[int] = None) -> int:

        result = super().access(op, addr, data)
        
        if op == "w":
            # Update cache if block is cached
            if addr in self.range_cache:
                self.range_cache[addr] = data
            self.blocks.add(addr)
            
        if self.access_counter >= self.eviction_rate:
            self.BatchEvict()
            
        return result
        
    def clear_range_cache(self):
        
        self.BatchEvict()
        self.range_cache.clear() 
