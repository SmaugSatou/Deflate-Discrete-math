# Deflate

## About the Project
This project implements **Huffman coding** and **LZ77 compression**, two fundamental algorithms for data compression.

## Features
- **Huffman Coding**
  - Builds an optimal binary tree for encoding data
  - Supports encoding and decoding operations

- **LZ77 Compression**
  - Implements LZ77 with a sliding window of 32 KB
  - Compresses input data into offset-length-character tuples
  - Supports decompression back to the original text

## Getting Started
### Requirements
To build and run this project, ensure you have:
- **C++17 or later**
- A compatible **C++ compiler** (GCC, Clang, MSVC, etc.)
- **CMake**

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/SmaugSatou/Deflate-Discrete-math.git
   cd Deflate-Discrete-math
   ```

### Unix (Linux, Macos)

2. Build the project:
   ```bash
   mkdir build && cd build
   cmake .. && make
   ```

### Windows 

2. Build the project:
   ```
   mkdir build
   cd build
   cmake ..
   ```
   ```
   cmake --build . --config Debug
   ```
   **or** 
   ```
   cmake --build . --config Release
   ```
3. Usage:
   ```
   ./Debug/deflate.exe <action> <inputFilePath> <compressedFilePath> <decompressedFilePath>
   ```
### Usage examples
Compressing data
```
./Debug/deflate.exe compress <./input.txt> <./compressed_output_.deflate>
```
Decompressing data
```
./Debug/huffman.exe decompress -  <./compressed_output_.deflate> <./decompressed__output.txt>
```