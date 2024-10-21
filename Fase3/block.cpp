#include "block.h"
#include "sha256.h"
#include <sstream>
#include <iomanip>

Block::Block(int data) {
    this->data = data;
    index = 0;
    nonce = 0;
    previousHash = "";
    hash = "";
    time = "";
    rootMerkle = "";
    difficulty = 4;
    manipulado = false;  // Inicializamos el valor de manipulado
}

std::string Block::generateHash() {
    try {
        std::string all = to_string(index) + time + to_string(nonce) + rootMerkle + previousHash;
        return sha256(all);
    } catch (const std::exception& e) {
        std::cerr << "Error generando el hash del bloque: " << e.what() << std::endl;
        return "";
    }
}


void Block::mineBlock(){
    string blockHash = generateHash();
    string target(this->difficulty, '0');

    while(blockHash.substr(0, this->difficulty) != target){
        this->nonce++;
        blockHash = generateHash();
    }
    this->hash = blockHash;
}
