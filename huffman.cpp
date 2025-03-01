#include <unordered_map>
#include <vector>
#include <queue>
#include <functional>
#include <string>
#include <fstream>
#include <iostream>

class Huffman {
private:

    struct Node {
        unsigned char character;
        int rate;
        Node* left;
        Node* right;

        Node(char character, int rate) : character(character), rate(rate), left(nullptr), right(nullptr) {}
        Node(int rate) : character(0), rate(rate), left(nullptr), right(nullptr) {}

        bool isLeaf() const {
            return !left && !right;
        }
    };

    std::unordered_map<char, std::string> codeDict;

    /**
     * @brief Builds the Huffman tree based on character frequencies.
     * @param data The input string for which the tree is built.
     * @return A pointer to the root of the constructed Huffman tree.
     */
    Node* buildTree(const std::string& data) {
        std::unordered_map<char, int> rateCount;

        for (const char& ch : data) {
            rateCount[ch]++;
        }

        std::priority_queue<Node*, std::vector<Node*>, std::function<bool(const Node*, const Node*)>> minRate(
            [](const Node* left, const Node* right) {
                return left->rate > right->rate;
            }
        );

        for (const auto& chRate : rateCount) {
            minRate.push(new Node(chRate.first, chRate.second));
        }

        while (minRate.size() > 1) {
            Node* left = minRate.top();
            minRate.pop();

            Node* right = minRate.top();
            minRate.pop();

            Node* parent = new Node('+', left->rate + right->rate);
            parent->left = left;
            parent->right = right;

            minRate.push(parent);
        }

        return minRate.top();
    }

    /**
     * @brief Recursively fills the Huffman code dictionary.
     * @param node The current node being processed.
     * @param code The Huffman code generated so far.
     */
    void fillCodeDict(Node* node, const std::string& code = "") {
        if (!node) {
            return;
        }

        if (node->character != '+') {
            codeDict[node->character] = code;
        }

        fillCodeDict(node->left, code + '0');
        fillCodeDict(node->right, code + '1');
    }

    /**
     * @brief Recursively deletes the Huffman tree to free memory.
     * @param node The current node to be deleted.
     */
    void destroyTree(Node* node) {
        if (!node) {
            return;
        }

        destroyTree(node->left);
        destroyTree(node->right);

        delete node;
    }

    /**
     * @brief Saves the Huffman tree to a file in a binary format.
     * @param root The root node of the Huffman tree.
     * @param outFile The output file stream to write the tree data to.
     */
    void saveHuffmanTree(Node* root, std::ofstream& outFile) {
        if (!root) {
            return;
        }

        if (root->isLeaf()) {
            outFile.put(0x01);
            outFile.write(reinterpret_cast<const char*>(&root->character), sizeof(root->character));
            outFile.write(reinterpret_cast<const char*>(&root->rate), sizeof(root->rate));
        }
        else {
            outFile.put(0x00);
            outFile.write(reinterpret_cast<const char*>(&root->rate), sizeof(root->rate));
        }

        saveHuffmanTree(root->left, outFile);
        saveHuffmanTree(root->right, outFile);
    }

    /**
     * @brief Recursively loads the Huffman tree from a file.
     * @param inFile The input file stream from which the Huffman tree is loaded.
     * @param remainingNodes A reference to the number of remaining nodes to be loaded.
     * @return A pointer to the loaded Huffman tree's root node, or `nullptr` if an error occurs.
     */
    Node* loadHuffmanTree(std::ifstream& inFile, int& remainingNodes) {
        if (remainingNodes <= 0) {
            return nullptr;
        }
        remainingNodes--;

        char flag;
        if (!inFile.get(flag)) {
            std::cerr << "Error: Unexpected EOF while reading flag." << std::endl;
            return nullptr;
        }

        if (flag == 1) {
            unsigned char byte;
            int frequency;

            if (!inFile.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
                std::cerr << "Error: Unexpected EOF while reading character." << std::endl;
                return nullptr;
            }

            if (!inFile.read(reinterpret_cast<char*>(&frequency), sizeof(frequency))) {
                std::cerr << "Error: Unexpected EOF while reading frequency." << std::endl;
                return nullptr;
            }

            return new Node(byte, frequency);
        }
        else {
            int frequency;

            if (!inFile.read(reinterpret_cast<char*>(&frequency), sizeof(frequency))) {
                std::cerr << "Error: Unexpected EOF while reading frequency for internal node." << std::endl;
                return nullptr;
            }

            Node* left = loadHuffmanTree(inFile, remainingNodes);
            Node* right = loadHuffmanTree(inFile, remainingNodes);

            Node* internalNode = new Node(frequency);
            internalNode->left = left;
            internalNode->right = right;
            return internalNode;
        }
    }

    /**
     * @brief Recursively counts the nodes in the Huffman tree.
     * @param root The root node of the Huffman tree.
     * @param count A reference to the node count.
     */
    void countNodes(Node* root, int& count) {
        if (root == nullptr) {
            return;
        }

        count++;
        countNodes(root->left, count);
        countNodes(root->right, count);
    }

public:
    Node* root;

    /**
     * @brief Recursively prints the structure of the Huffman tree.
     * @param root The current node of the Huffman tree to be printed.
     * @param level The current level in the tree, used for indentation. Default value is 0.
     */
    void printTree(Node* root, int level = 0) {
        if (root == nullptr) {
            return;
        }

        for (int i = 0; i < level; ++i) {
            std::cout << "    ";
        }

        if (root->isLeaf()) {
            std::cout << "Leaf: '" << root->character << "' with frequency: " << root->rate << "\n";
        }
        else {
            std::cout << "Internal node with frequency: " << root->rate << "\n";
        }

        printTree(root->left, level + 1);
        printTree(root->right, level + 1);
    }

    /**
     * @brief Constructor initializes the root to nullptr.
     */
    Huffman() : root(nullptr) {}

    /**
     * @brief Destructor frees memory occupied by the Huffman tree.
     */
    ~Huffman() {
        this->destroyTree(root);
    }

    /**
     * @brief Builds the Huffman tree and generates codes for encoding.
     * @param data The input string to be processed.
     */
    void build(const std::string& data) {
        this->root = buildTree(data);
        fillCodeDict(root);
    }

    /**
     * @brief Encodes a given string using Huffman encoding.
     * @param data The string to encode.
     * @return The Huffman-encoded binary string.
     */
    std::string encode(const std::string& data) {
        std::string encodedData;

        for (char ch : data) {
            encodedData += codeDict[ch];
        }

        return encodedData;
    }

    /**
     * @brief Decodes a Huffman-encoded string.
     * @param encodedData The encoded binary string.
     * @return The original decoded string.
     */
    std::string decode(const std::string& encodedData) {
        std::string decodedData;
        Node* node = root;

        for (char bit : encodedData) {
            if (bit == '0') {
                node = node->left;
            }
            else {
                node = node->right;
            }


            if (node->isLeaf()) {
                decodedData += node->character;
                node = root;
            }
        }

        return decodedData;
    }

    /**
     * @brief Saves the Huffman tree to a file.
     * @param filename The file to save the Huffman tree.
     */
    void saveHuffmanTreeToFile(const std::string filename) {
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile) {
            return;
        }

        int nodeCount = 0;
        countNodes(root, nodeCount);
        outFile.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));

        saveHuffmanTree(this->root, outFile);

        outFile.close();
    }

    /**
     * @brief Loads a Huffman tree from a binary file.
     * @param filename The path to the file containing the serialized Huffman tree.
     * @return std::streampos The current position in the file after the Huffman tree has been loaded.
     */
    std::streampos loadTreeFromFile(const std::string& filename) {
        std::ifstream inFile(filename, std::ios::binary);

        int nodeCount;
        inFile.read(reinterpret_cast<char*>(&nodeCount), sizeof(nodeCount));

        this->root = loadHuffmanTree(inFile, nodeCount);
        std::streampos currentPos = inFile.tellg();

        inFile.close();
        return currentPos;
    }
};
