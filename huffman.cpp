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
        char character;
        int rate;
        Node* left;
        Node* right;

        /**
         * @brief Constructs a Node with a given character and frequency.
         * @param character The character stored in this node.
         * @param rate The frequency of the character.
         */
        Node(char character, int rate) : character(character), rate(rate), left(nullptr), right(nullptr) {}
    };

    Node* root;
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
    * @brief Saves the Huffman tree structure to a file.
    * @param root The root of the Huffman tree.
    * @param outFile The output file stream.
    */
    void saveHuffmanTree(Node* root, std::ofstream& outFile) {
        if (root == nullptr) {
            return;
        }

        if (root->left == nullptr && root->right == nullptr) {
            outFile << "1 " << root->character << " " << root->rate << "\n";
        }
        else {
            outFile << "0 " << root->rate << "\n";
        }

        saveHuffmanTree(root->left, outFile);
        saveHuffmanTree(root->right, outFile);
    }

    /**
     * @brief Loads a Huffman tree from a file.
     * @param inFile The input file stream.
     * @return A pointer to the root of the reconstructed Huffman tree.
     */
    Node* loadHuffmanTree(std::ifstream& inFile) {
        char flag;
        inFile >> flag;  

        if (flag == '1') {
            inFile.get();
            char symbol = inFile.get();
            int frequency;
            inFile >> frequency;

            return new Node(symbol, frequency);
        }
        else if (flag == '0') {
            int frequency;
            inFile >> frequency;

            Node* left = loadHuffmanTree(inFile);
            Node* right = loadHuffmanTree(inFile);

            Node* internalNode = new Node(0, frequency);
            internalNode->left = left;
            internalNode->right = right;
            return internalNode;
        }

        return nullptr;
    }

public:
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

            if (!node->left && !node->right) {
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
        saveHuffmanTree(this->root, outFile);
        outFile.close();
    }

    /**
     * @brief Loads a Huffman tree from a file.
     * @param filename The file from which to load the tree.
     */
    void loadHuffmanTreeFromFile(const std::string& filename) {
        std::ifstream inFile(filename, std::ios::binary);
        this->root = loadHuffmanTree(inFile);
        if (!root) {
            std::cout << "None";
        }
        inFile.close();
    }
};
