#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <QString>


class Neighbor {
public:
    QString value;  // Cambiar de int a QString
    Neighbor* next;

    Neighbor(const QString& val) {
        value = val;
        next = nullptr;
    }
};

class node
{

public:
    QString value;
    node* next;
    Neighbor* neighbors;

    node(const QString& val);
    void insertNeighbor(const QString& destiny);
    void graphNeighbors(std::ofstream &out);


};

using namespace std;

class nodeTree {
public:
    string value;
    nodeTree* left;
    nodeTree* right;
    string originalData;

    nodeTree(string value) {
        this->value = value;
        this->left = nullptr;
        this->right = nullptr;
        this->originalData = "";
    }
};


#endif // NODE_H
