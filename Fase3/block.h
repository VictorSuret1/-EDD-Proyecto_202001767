#ifndef BLOCK_H
#define BLOCK_H
#include <iostream>

using namespace std;

class Block
{
private:

public:
    string generateHash();

    Block(int data);
    void mineBlock();

    int index;
    int nonce;
    int difficulty;
    std::string previousHash;
    std::string hash;
    std::string time;
    std::string data;
    std::string rootMerkle;
    bool manipulado;

    // pointers
    Block* next;
    Block* back;
};

#endif // BLOCK_H
