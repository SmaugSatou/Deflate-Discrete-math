#include <fstream>
#include <vector>
#include <iostream>
#include <bitset>

 /**
  * @brief Writes a compressed binary representation of a bitstring to a file.
  * @param bitString The string containing binary data ('0' and '1').
  * @param fileName The name of the output file (default: "compressed.deflate").
  */
static void writeCompressedData(const std::string& bitString, const std::string& fileName = "compressed.deflate") {
    std::ofstream file(fileName, std::ios::binary | std::ios::app);

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


/**
 * @brief Reads a compressed binary file and converts it back into a bitstring.
 * @param fileName The name of the file to read (default: "compressed.deflate").
 * @param pos A position of data skipping header info.
 * @return A string containing the extracted bit sequence.
 */
static std::string readCompressedData(const std::string& fileName = "compressed.deflate", std::streampos pos = 0) {
    std::ifstream file(fileName, std::ios::binary);

    if (!file) {
        std::cerr << "Error: file cannot be opened!" << std::endl;
        return "";
    }

    file.seekg(pos, std::ios::beg);

    std::vector<uint8_t> binaryData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::string bitString;

    for (uint8_t byte : binaryData) {
        bitString += std::bitset<8>(byte).to_string();
    }

    return bitString;
}
