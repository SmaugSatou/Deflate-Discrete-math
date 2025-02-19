#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <functional>

class Huffman {
private:
    struct Node {
        char character;
        int rate;
        Node* left;
        Node* right;

        Node(char character, int rate) : character(character), rate(rate), left(nullptr), right(nullptr) {}
    };

    Node* root;
    std::unordered_map<char, std::string> codeDict;

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

    void destroyTree(Node* node) {
        if (!node) {
            return;
        }

        destroyTree(node->left);
        destroyTree(node->right);

        delete node;
    }

public:
    Huffman() : root(nullptr) {}

    ~Huffman() {
        this->destroyTree(root);
    }

    void build(const std::string& data) {
        this->root = buildTree(data);
        fillCodeDict(root);
    }

    std::string encode(const std::string& data) {
        std::string encodedData;

        for (char ch : data) {
            encodedData += codeDict[ch];
        }

        return encodedData;
    }

    std::string decode(const std::string& encodedData) {
        std::string decodedData;
        Node* node = root;

        for (char bit : encodedData) {
            if (bit == '0') {
                node = node->left;
            } else {
                node = node->right;
            }

            if (!node->left && !node->right) {
                decodedData += node->character;
                node = root;
            }
        }

        return decodedData;
    }
};

class LZ77 {

};

static std::string deflateCompress(const std::string inputFilePath, const std::string outputFilePath) {

}

static std::string deflateDecompress(const std::string inputFilePath, const std::string outputFilePath) {

}

int main() {
    return 0;
}