#include "huffman.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <QString>
#include <QFile>

Huffman::Huffman()  {}

// Codificar la información utilizando Huffman
void Huffman::compress(const QString& input, const QString& outputPath) {
    std::string data = input.toStdString();
    std::unordered_map<char, int> freqMap;

    // Calcular la frecuencia de cada carácter
    for (char ch : data) {
        freqMap[ch]++;
    }

    // Crear una cola de prioridad
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> pq;
    for (auto pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    // Construir el árbol de Huffman
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* newNode = new HuffmanNode('\0', left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }

    HuffmanNode* root = pq.top();

    // Generar los códigos de Huffman
    std::unordered_map<char, std::string> huffmanCodes;
    generateCodes(root, "", huffmanCodes);

    // Serializar el árbol
    std::string serializedTree;
    serializeTree(root, serializedTree);

    // Codificar los datos
    std::string encodedData;
    for (char ch : data) {
        encodedData += huffmanCodes[ch];
    }

    // Escribir la información comprimida en el archivo
    std::ofstream outFile(outputPath.toStdString(), std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "No se pudo abrir el archivo para escritura: " << outputPath.toStdString() << std::endl;
        return;
    }

    // Guardar la serialización del árbol seguida de los datos codificados
    outFile << serializedTree << encodedData;
    outFile.close();
}

// Decodificar la información utilizando Huffman
QString Huffman::decompress(const QString& inputPath) {
    std::ifstream inFile(inputPath.toStdString(), std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "No se pudo abrir el archivo para lectura: " << inputPath.toStdString() << std::endl;
        return QString();
    }

    // Leer los datos comprimidos
    std::string encodedData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    // Imprimir los datos leídos para verificar si se leyeron correctamente
    std::cout << "Datos comprimidos leídos: " << encodedData << std::endl;

    // Verificar si hay datos suficientes para deserializar el árbol
    if (encodedData.empty()) {
        std::cerr << "Error: El archivo comprimido está vacío." << std::endl;
        return QString();
    }

    // Reconstruir el árbol de Huffman desde los datos serializados
    int index = 0;
    HuffmanNode* root = deserializeTree(encodedData, index);
    if (!root) {
        std::cerr << "Error: No se pudo reconstruir el árbol de Huffman." << std::endl;
        return QString();
    }

    // El resto es la información comprimida
    std::string compressedData = encodedData.substr(index);

    // Decodificar los datos usando el árbol de Huffman reconstruido
    std::string decodedData;
    HuffmanNode* currentNode = root;

    for (char bit : compressedData) {
        if (bit == '0') {
            currentNode = currentNode->left;
        } else {
            currentNode = currentNode->right;
        }

        if (!currentNode) {
            std::cerr << "Error: Nodo nulo encontrado durante la descompresión." << std::endl;
            return QString();
        }

        // Si llegamos a un nodo hoja
        if (!currentNode->left && !currentNode->right) {
            decodedData += currentNode->data;
            currentNode = root;
        }
    }

    return QString::fromStdString(decodedData);
}

void Huffman::serializeTree(HuffmanNode* root, std::string& out) {
    if (!root) return;

    if (!root->left && !root->right) {
        // Nodo hoja, agregar '1' y luego el carácter
        out += "1";
        out += root->data;
    } else {
        // Nodo interno, agregar '0'
        out += "0";
    }

    serializeTree(root->left, out);
    serializeTree(root->right, out);
}

void Huffman::generateCodes(HuffmanNode* root, std::string code, std::unordered_map<char, std::string>& huffmanCodes) {
    if (!root) return;

    // Si es un nodo hoja
    if (!root->left && !root->right) {
        huffmanCodes[root->data] = code;
    }

    // Recorre hacia la izquierda
    generateCodes(root->left, code + "0", huffmanCodes);
    // Recorre hacia la derecha
    generateCodes(root->right, code + "1", huffmanCodes);
}

HuffmanNode* Huffman::deserializeTree(const std::string& treeData, int& index) {
    if (index >= treeData.size()) {
        std::cerr << "Error: Índice fuera de rango al deserializar el árbol." << std::endl;
        return nullptr;
    }

    // Nodo hoja
    if (treeData[index] == '1') {
        if (++index >= treeData.size()) {
            std::cerr << "Error: Índice fuera de rango al intentar leer un nodo hoja." << std::endl;
            return nullptr;
        }

        char data = treeData[index++];
        return new HuffmanNode(data, 0);
    }

    // Nodo interno
    if (treeData[index] == '0') {
        index++;
        HuffmanNode* left = deserializeTree(treeData, index);
        HuffmanNode* right = deserializeTree(treeData, index);

        if (!left || !right) {
            std::cerr << "Error: Nodo hijo nulo encontrado durante la deserialización." << std::endl;
            return nullptr;
        }

        HuffmanNode* newNode = new HuffmanNode('\0', 0); // Nodo interno, sin valor específico
        newNode->left = left;
        newNode->right = right;

        return newNode;
    }

    std::cerr << "Error: Formato inesperado durante la deserialización." << std::endl;
    return nullptr;
}
