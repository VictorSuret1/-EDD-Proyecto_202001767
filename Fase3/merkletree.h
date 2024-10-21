#ifndef MERKLETREE_H
#define MERKLETREE_H
#include "node.h"

class MerkleTree
{
private:
    vector<nodeTree*> *getList();
    void getList(vector<nodeTree*> *nodes, nodeTree* current);
    void buildTree(vector<nodeTree*> *nodes);

    void writeNode(ofstream &file, nodeTree *node);
public:
    MerkleTree(/* args */);
    std::string getRootHash();

    nodeTree* root;
    void insert(string value);
    void generateGraph(string &filename);
};

#endif // MERKLETREE_H
