#include <fstream>
#include <vector>
#include <iostream>
#include <bitset>

static void writeCompressedData(const std::string& bitString, const std::string& fileName = "compressed.deflate") {
    std::ofstream file(fileName, std::ios::binary);

    if (!file) {
        std::cerr << "Error: file cannot be open!";
        return;
    }

    std::vector<uint8_t> binaryData;
    uint8_t currentByte = 0;
    int bitCount = 0;

    for (const char& bit : bitString) {
        currentByte = (currentByte << 1) | (bit - '0');
        bitCount++;

        if (bitCount == 8) {
            binaryData.push_back(currentByte);
            currentByte = 0;
            bitCount = 0;
        }
    }

    if (bitCount > 0) {
        currentByte <<= (8 - bitCount);
        binaryData.push_back(currentByte);
    }

    file.write(reinterpret_cast<const char*>(binaryData.data()), binaryData.size());
    file.close();
}

static std::string readCompressedData(const std::string& fileName = "compressed.deflate") {
    std::ifstream file(fileName, std::ios::binary);

    if (!file) {
        std::cerr << "Error: file cannot be open!";
        return "";
    }

    std::vector<uint8_t> binaryData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::string bitString;
    for (uint8_t byte : binaryData) {
        bitString += std::bitset<8>(byte).to_string();
    }

    return bitString;
}