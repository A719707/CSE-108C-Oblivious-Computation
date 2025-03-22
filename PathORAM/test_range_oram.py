from range_oram import RangeORAM
from server import Server
import random

def test_basic_operations():
    print("\nTesting Ops")
    num_blocks = 16
    roram = RangeORAM(numBlocks=num_blocks, bucketSize=4, range_size=4)
    server = Server(roram.height, roram.bucketSize)
    roram.connectServer(server)
    roram.initializeTree()

    # Test 1
    print("\nWriting values to blocks 0-5")
    for i in range(6):
        roram.Access(i, "w", i * 10)
        print(f"Wrote value {i * 10} to block {i}")

    # Test 2
    print("\nReading individual blocks")
    for i in range(6):
        value = roram.Access(i, "r")
        print(f"Read value {value} from block {i}")

    # Test 3
    print("\nReading range [2-5]")
    range_result = roram.ReadRange(2, 5)
    print("Range read result:", range_result)

    return roram

def test_batch_eviction():
    print("\nTesting Batch Eviction")
    roram = RangeORAM(numBlocks=8, bucketSize=4, range_size=2)
    server = Server(roram.height, roram.bucketSize)
    roram.connectServer(server)
    roram.initializeTree()

    print("\nReading back values after batch eviction...")
    # Write enough values to trigger batch eviction
    for i in range(roram.eviction_rate + 1):
        value = i * 5
        roram.Access(i, "w", value)
        print(f"Wrote value {value} to block {i}")

    print("\nReading back values after batch eviction...")
    for i in range(roram.eviction_rate + 1):
        value = roram.Access(i, "r")
        print(f"Read value {value} from block {i}")

def test_range_queries():
    print("\nTesting Range Queries")
    roram = RangeORAM(numBlocks=32, bucketSize=4, range_size=8)
    server = Server(roram.height, roram.bucketSize)
    roram.connectServer(server)
    roram.initializeTree()

    # Write some sequential values
    print("\nWriting sequential values")
    for i in range(20):
        roram.Access(i, "w", i * 2)

    # Test different range sizes
    ranges = [(0, 4), (5, 10), (15, 19)]
    for start, end in ranges:
        print(f"\nReading range [{start}-{end}]...")
        result = roram.ReadRange(start, end)
        print(f"Range query result: {result}")

def interactive_test():
    print("\nTesting")
    num_blocks = int(input("Enter number of blocks: "))
    bucket_size = int(input("Enter bucket size: "))
    range_size = int(input("Enter range size: "))

    roram = RangeORAM(numBlocks=num_blocks, bucketSize=bucket_size, range_size=range_size)
    server = Server(roram.height, roram.bucketSize)
    roram.connectServer(server)
    roram.initializeTree()

    while True:
        print("\nOps:")
        print("1. Write value")
        print("2. Read value")
        print("3. Read range")
        print("4. Exit")
        
        choice = input("Enter (1-4): ")
        
        if choice == "1":
            addr = int(input("Enter block: "))
            value = int(input("Enter value: "))
            roram.Access(addr, "w", value)
            print(f"Wrote value {value} to block {addr}")
            
        elif choice == "2":
            addr = int(input("Enter block: "))
            value = roram.Access(addr, "r")
            print(f"Read value {value} from block {addr}")
            
        elif choice == "3":
            start = int(input("Enter start: "))
            end = int(input("Enter end: "))
            result = roram.ReadRange(start, end)
            print(f"Range query result: {result}")
            
        elif choice == "4":
            break

if __name__ == "__main__":
    
    while True:
        print("\nTest Ops:")
        print("1. Run basic operations test")
        print("2. Run batch eviction test")
        print("3. Run range queries test")
        print("4. Run interactive test")
        print("5. Exit")
        
        choice = input("Enter choice (1-5): ")
        
        if choice == "1":
            test_basic_operations()
        elif choice == "2":
            test_batch_eviction()
        elif choice == "3":
            test_range_queries()
        elif choice == "4":
            interactive_test()
        elif choice == "5":
            break
        else:
            print("Invalid choice. Please try again.") 
