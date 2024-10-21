#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H
#include "block.h"
class Blockchain
{
private:


public:
    Block* last;
    int size;

    int getSize();
    string getPrehash();
    Blockchain(/* args */);
    Block* first;
    void insert(const std::string& data);
    void print();
    bool verificarIntegridad();
    void guardarBloqueEnArchivo(Block* bloque);
    Block* getLastBlock();
    void restaurarPublicacionesDesdeBloque(Block* bloque);
    void cargarBlockchain();
    void limpiarBlockchain();
};

#endif // BLOCKCHAIN_H
