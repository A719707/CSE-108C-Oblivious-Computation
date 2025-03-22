# rORAM Implementation

This repository contains an implementation of **Range-ORAM (rORAM)**, an Oblivious RAM (ORAM) construction designed to efficiently support range queries while minimizing disk seeks. The implementation builds on the classic Path ORAM and introduces features like multiple sub-ORAMs, batch evictions, and bit-reversed path ordering to improve performance for range queries.

---

## Table of Contents
1. [Overview](#overview)
2. [Key Features](#key-features)
3. [Implementation Details](#implementation-details)
4. [How to Build and Run](#how-to-build-and-run)
5. [Testing the Implementation](#testing-the-implementation)
   - [CorrectnessTester1](#correctnesstester1)
   - [CorrectnessTester2](#correctnesstester2)
   - [CorrectnessTester3](#correctnesstester3)
   - [CorrectnessTester4](#correctnesstester4)
   - [CorrectnessTester5](#correctnesstester5)
6. [Performance Benchmarking](#performance-benchmarking)
7. [Documentation](#documentation)
8. [Contributing](#contributing)
9. [License](#license)

---

## Overview
ORAM is a cryptographic primitive that hides access patterns to data stored on an untrusted server. rORAM extends this concept to efficiently handle **range queries**, which are common in applications like databases, file systems, and video streaming.

This implementation includes:
- A **coordinator layer** to manage multiple sub-ORAMs.


---

## How to Build and Run
### Prerequisites
- C++ compiler (e.g., `g++`).
- CMake (optional, for building the project).

### Build Instructions
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/rORAM.git
   cd rORAM

2. Build the project
mkdir build
cd build
cmake ..
make

3. Run the Correctness Testers
./CorrectnessTester1
./CorrectnessTester2
./CorrectnessTester3
./CorrectnessTester4
./CorrectnessTester5

Acknowledgements

This implementation of rORAM was built upon the efforts of a PathORAM construction by Jasleen1 and young-du, and I would like to give a special thanks to their contributions to the development of this project, as well as the random number generator created by Dumas. 

Contact: aandhare@ucsc.edu

Github: A719707









- Support for **range queries** and **batch evictions**.
- Five correctness testers to validate the implementation.

---

## Key Features
- **Multiple Sub-ORAMs**: Each sub-ORAM handles a specific range size (e.g., 1, 2, 4, 8, etc.).
- **Range Queries**: The `ReadRange` function fetches a contiguous range of blocks efficiently.
- **Batch Evictions**: The `batchEvict` function minimizes disk seeks by evicting multiple blocks in a single operation.
- **Bit-Reversed Path Ordering**: Improves locality and reduces disk seeks.

---

## Implementation Details
### Core Components
1. **rORAM Coordinator Layer**:
   - Manages multiple sub-ORAMs.
   - Implements `ReadRange`, `WriteBlock`, and `batchEvict`.

2. **Modified ORAM Classes**:
   - `OramDeterministic` and `OramReadPathEviction` are extended to support rORAM features.

3. **Correctness Testers**:
   - Five testers (`CorrectnessTester1` to `CorrectnessTester5`) to validate different aspects of the implementation.

---

### File Structure
