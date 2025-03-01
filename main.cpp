#include <iostream>

#include "huffman.cpp"
#include "lz77.cpp"
#include "filemanager.cpp"

/**
 * @brief Compresses a file using LZ77 followed by Huffman coding and writes the output to a compressed file.
 * @param inputFilePath The path to the input file to be compressed.
 * @param outputFilePath The path where the compressed file will be saved.
 * @param huffman A reference to a Huffman object used for encoding.
 * @return The compressed Huffman-encoded bitstring.
 */
static std::string deflateCompress(const std::string inputFilePath, const std::string outputFilePath, Huffman& huffman) {
    std::ifstream inputFile(inputFilePath);

    std::string inputData((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    Lz77 lz77;
    std::vector<Lz77::Lz77Code> lz77CompressedData = lz77.lz77Compress(inputData);

    std::string lz77CompressedString = lz77.compressedToString(lz77CompressedData);

    huffman.build(lz77CompressedString);

    std::string huffmanCompressedData = huffman.encode(lz77CompressedString);
    huffman.saveHuffmanTreeToFile(outputFilePath);

    writeCompressedData(huffmanCompressedData, outputFilePath);

    return huffmanCompressedData;
}

/**
 * @brief Decompresses a file using Huffman decoding followed by LZ77 decompression.
 * @param inputFilePath The path to the compressed file.
 * @param outputFilePath The path where the decompressed file will be saved.
 * @param huffman A reference to a Huffman object used for decoding.
 * @param pos A position of data skipping header info.
 * @return The decompressed string.
 */
static std::string deflateDecompress(const std::string inputFilePath, const std::string outputFilePath, Huffman& huffman, std::streampos pos) {
    std::string huffmanCompressedData = readCompressedData(inputFilePath, pos);
    
    std::string lz77CompressedString = huffman.decode(huffmanCompressedData);

    std::string decompressedData = Lz77::lz77DecompressFromString(lz77CompressedString);
       
    std::ofstream outputFile(outputFilePath);
    outputFile << decompressedData;
    outputFile.close();

    return decompressedData;
}

/**
 * @brief Main function that serves as the entry point for the compression and decompression program.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Returns 0 on successful execution, 1 on errors.
 */
int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <action> <inputFilePath> <compressedFilePath> <decompressedFilePath>\n";
        std::cerr << "Action options: compress | decompress\n";
        return 1;
    }

    std::string action = argv[1];
    std::string inputFilePath = argv[2];
    std::string compressedFilePath = argv[3];
    std::string decompressedFilePath = (argc > 4) ? argv[4] : "";;

    Huffman huffman;

    if (action == "compress") {
        std::cout << "Compressing...\n";

        std::string compressResult = deflateCompress(inputFilePath, compressedFilePath, huffman);

        std::cout << "Compression done! Compressed data size: " << compressResult.size() << " bytes.\n";
    } 
    else if (action == "decompress") {
        if (decompressedFilePath.empty()) {
            std::cerr << "For decompression, provide the decompressed file path.\n";
            return 1;
        }

        std::streampos pos = huffman.loadTreeFromFile(compressedFilePath);

        std::cout << "Decompressing...\n";

        std::string decompressResult = deflateDecompress(compressedFilePath, decompressedFilePath, huffman, pos);

        std::cout << "Decompression done! Output saved to: " << decompressedFilePath << "\n";
    }
    else {
        std::cerr << "Invalid action. Use 'compress' or 'decompress'.\n";
        return 1;
    }

    return 0;
}
