#include "vector"
#include "iostream"
#include "string"
#include "sstream"

class Lz77 {
private:
	const int WINDOW_SIZE = 32768;

public:
    struct Lz77Code {
        int offSet;
        int length;
        char nextChar;

        Lz77Code(int offSet, int length, char nextChar) : offSet(offSet), length(length), nextChar(nextChar) {};
    };

    /**
     * Compresses a given string using LZ77 algorithm.
     * @param data The input string to compress.
     * @return A vector of Lz77Code structs representing the compressed data.
     */
    std::vector<Lz77Code> lz77Compress(const std::string& data) {
        std::vector<Lz77Code> compressedData;
        int index = 0;

        while (index < data.size()) {
            int bestCopyLength = 0;
            int currOffSet = 0;
            int startingWindowIndex = (index < WINDOW_SIZE) ? 0 : index - WINDOW_SIZE;

            for (int windowIndex = startingWindowIndex; windowIndex < index; ++windowIndex) {
                int currMatchLength = 0;

                while (index + currMatchLength < data.size() &&
                    data[index + currMatchLength] == data[windowIndex + currMatchLength]) {
                    currMatchLength++;

                    if (windowIndex + currMatchLength >= index) {
                        break;
                    }
                }

                if (currMatchLength > bestCopyLength) {
                    bestCopyLength = currMatchLength;
                    currOffSet = index - windowIndex;
                }
            }

            char nextChar = (index + bestCopyLength < data.size()) ? data[index + bestCopyLength] : '\0';

            compressedData.emplace_back(currOffSet, bestCopyLength, nextChar);
            index += bestCopyLength + 1;
        }

        return compressedData;
    }

    /**
     * Decompresses a vector of Lz77Code structures back into the original string.
     * @param codes The vector of Lz77Code structs representing compressed data.
     * @return The decompressed string.
     */
    static std::string lz77Decompress(const std::vector<Lz77Code>& codes) {
        std::string decompressedText;
        for (const Lz77Code& code : codes) {
            if (code.length > 0) {
                size_t startPos = decompressedText.size() - code.offSet;
                for (size_t i = 0; i < code.length; ++i) {
                    decompressedText += decompressedText[startPos + i];
                }
            }

            decompressedText += code.nextChar;
        }
        return decompressedText;
    }

    /**
    * Converts compressed LZ77 data into a string representation.
    * @param codes The vector of Lz77Code structs.
    * @return A string representation of the compressed data.
    */
    std::string compressedToString(const std::vector<Lz77Code>& codes) {
        std::stringstream ss;

        for (const auto& token : codes) {
            ss << "(" << token.offSet << "," << token.length << "," << token.nextChar << ")";
        }

        return ss.str();
    }

    /**
    * Parses a string representation of compressed LZ77 data back into Lz77Code structures.
    * @param tokenStr The string representation of compressed data.
    * @return A vector of Lz77Code structs.
    */
    static std::vector<Lz77Code> parseTokensFromString(const std::string& tokenStr) {
        std::vector<Lz77Code> tokens;
        size_t pos = 0;

        while (pos < tokenStr.size()) {
            size_t start = tokenStr.find('(', pos);
            if (start == std::string::npos) {
                break;
            }

            size_t end = tokenStr.find(')', start);
            if (end == std::string::npos) {
                throw std::runtime_error("Invalid token format: missing closing parenthesis");
            }

            std::string tokenContent = tokenStr.substr(start + 1, end - start - 1);
            size_t firstComma = tokenContent.find(',');
            size_t secondComma = tokenContent.find(',', firstComma + 1);

            if (firstComma == std::string::npos || secondComma == std::string::npos) {
                throw std::runtime_error("Invalid token format: missing commas");
            }

            try {
                size_t offset = std::stoul(tokenContent.substr(0, firstComma));
                size_t length = std::stoul(tokenContent.substr(firstComma + 1, secondComma - firstComma - 1));

                if (secondComma + 1 >= tokenContent.size() || tokenContent[secondComma + 1] == ')') {
                    throw std::runtime_error("Invalid token format: missing nextChar");
                }

                char nextChar = tokenContent[secondComma + 1];
                tokens.emplace_back(offset, length, nextChar);
            }
            catch (const std::exception&) {
                throw std::runtime_error("Invalid token format: conversion error");
            }

            pos = end + 1;
        }

        return tokens;
    }

    /**
     * Decompresses a string representation of LZ77 compressed data.
     * @param tokenStr The string representation of compressed data.
     * @return The decompressed original string.
     */
    static std::string lz77DecompressFromString(const std::string& tokenStr) {
        std::vector<Lz77Code> tokens = parseTokensFromString(tokenStr);
        return lz77Decompress(tokens);
    }

};
