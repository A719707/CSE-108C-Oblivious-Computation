import unittest
from client import Client
from server import Server
from block import Block

class TestClientServerORAM(unittest.TestCase):

    def setUp(self):
        self.numBlocks = 16
        self.bucketSize = 4
        self.client = Client(self.numBlocks, self.bucketSize)
        self.server = Server(self.client.height, self.bucketSize)
        self.client.connectServer(self.server)
        self.client.initializeTree()

    def test_write_and_read(self):
        for i in range(self.numBlocks):
            self.client.access("w", i, i * 10)
            read_data = self.client.access("r", i)
            self.assertEqual(read_data, i * 10)

    def test_overwrite_data(self):
        self.client.access("w", 5, 50)
        self.client.access("w", 5, 100)
        read_data = self.client.access("r", 5)
        self.assertEqual(read_data, 100)

    def test_read_non_existent_data(self):
        read_data = self.client.access("r", 10)
        self.assertEqual(read_data, 0)

    def test_dummy_blocks(self):
        dummy_block = self.client.createDummyBlock()
        self.assertTrue(self.client.isDummy(dummy_block))

    # def test_encryption_decryption(self):
    #     block = Block(7, 777)
    #     encrypted_block = self.client.encryptBlock(block)
    #     decrypted_block = self.client.readBucket(self.server.getBucket(0, 0))
        
    #     self.assertIsInstance(encrypted_block, Block)
    #     self.assertEqual(block.getKey(), 7)
    #     self.assertEqual(block.getData(), 777)

if __name__ == '__main__':
    unittest.main()