#include <iostream>

#include "huffman.cpp"
#include "lz77.cpp"
#include "filemanager.cpp"

static std::string deflateCompress(const std::string inputFilePath, const std::string outputFilePath, Huffman& huffman) {
    std::ifstream inputFile(inputFilePath);

    std::string inputData((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    Lz77 lz77;
    std::vector<Lz77::Lz77Code> lz77CompressedData = lz77.lz77Compress(inputData);

    std::string lz77CompressedString = lz77.compressedToString(lz77CompressedData);

    huffman.build(lz77CompressedString);

    std::string treeFilePath = outputFilePath;
    if (treeFilePath.size() > 8 && treeFilePath.substr(treeFilePath.size() - 8) == ".deflate") {
        treeFilePath = treeFilePath.substr(0, treeFilePath.size() - 8);
    }

    treeFilePath += ".tree";

    std::string huffmanCompressedData = huffman.encode(lz77CompressedString);
    huffman.saveHuffmanTreeToFile(treeFilePath);


    writeCompressedData(huffmanCompressedData, outputFilePath);

    return huffmanCompressedData;
}

static std::string deflateDecompress(const std::string inputFilePath, const std::string outputFilePath, Huffman& huffman) {
    std::string huffmanCompressedData = readCompressedData(inputFilePath);
    
    std::string lz77CompressedString = huffman.decode(huffmanCompressedData);

    std::string decompressedData = Lz77::lz77DecompressFromString(lz77CompressedString);
       
    std::ofstream outputFile(outputFilePath);
    outputFile << decompressedData;
    outputFile.close();

    return decompressedData;
}

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
    } else if (action == "decompress") {
        if (decompressedFilePath.empty()) {
            std::cerr << "For decompression, provide the decompressed file path.\n";
            return 1;
        }

        std::string treeFilePath = compressedFilePath;
        if (treeFilePath.size() > 8 && treeFilePath.substr(treeFilePath.size() - 8) == ".deflate") {
            treeFilePath = treeFilePath.substr(0, treeFilePath.size() - 8);
        }
        treeFilePath += ".tree";

        std::ifstream treeFile(treeFilePath, std::ios::binary);
        if (treeFile) {
            std::cout << "Loading Huffman tree from: " << treeFilePath << '\n';
            huffman.loadHuffmanTreeFromFile(treeFilePath);
        } else {
            std::cerr << "Error: Huffman tree file not found. Unable to decompress.\n";
            return 1;
        }

        std::cout << "Decompressing...\n";
        std::string decompressResult = deflateDecompress(compressedFilePath, decompressedFilePath, huffman);
        std::cout << "Decompression done! Output saved to: " << decompressedFilePath << "\n";
    }
    else {
        std::cerr << "Invalid action. Use 'compress' or 'decompress'.\n";
        return 1;
    }

    return 0;
}
