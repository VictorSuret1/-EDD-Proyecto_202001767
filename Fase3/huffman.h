#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <vector>
#include <QString>
#include <QFile>
#include <unordered_map>

// Nodo del árbol de Huffman
struct HuffmanNode {
    char data;
    int frequency;
    HuffmanNode *left, *right;

    HuffmanNode(char data, int frequency) : data(data), frequency(frequency), left(nullptr), right(nullptr) {}
};
// Comparador para la cola de prioridad
struct Compare {
    bool operator()(HuffmanNode* left, HuffmanNode* right) {
        return left->frequency > right->frequency;
    }
};

class Huffman {
public:
    Huffman();  // Declaración del constructor predeterminado

    void compress(const QString& input, const QString& outputPath);
    QString decompress(const QString& inputPath);
    void generateCodes(HuffmanNode* root, std::string code, std::unordered_map<char, std::string>& huffmanCodes);
    void serializeTree(HuffmanNode* root, std::string& out);
    HuffmanNode* deserializeTree(const std::string& treeData, int& index);

private:

};

#endif // HUFFMAN_H
