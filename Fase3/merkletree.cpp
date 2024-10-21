#include "merkletree.h"
#include <fstream>
#include <cmath>
#include "sha256.h"
#include <filesystem>

MerkleTree::MerkleTree(){
    root = nullptr;
}

vector<nodeTree *> *MerkleTree::getList()
{
    vector<nodeTree *> *newListnodeTrees = new vector<nodeTree *>();
    getList(newListnodeTrees, root);

    return newListnodeTrees;
}
void MerkleTree::getList(vector<nodeTree *> *nodeTrees, nodeTree *current)
{
    if (current != nullptr)
    {
        getList(nodeTrees, current->left);
        if (current->right == nullptr && current->value != "-1")
        {
            nodeTrees->push_back(current);
        }
        getList(nodeTrees, current->right);
    }
}

void MerkleTree::insert(string value)
{
    nodeTree *newnodeTree = new nodeTree(value);
    newnodeTree->originalData = value;

    if (this->root == nullptr)
    {
        vector<nodeTree *> *nodeTrees = new vector<nodeTree *>();
        nodeTrees->push_back(newnodeTree);
        nodeTrees->push_back(new nodeTree("-1"));
        buildTree(nodeTrees);
    }
    else
    {
        vector<nodeTree *> *nodeTrees = getList();
        nodeTrees->push_back(newnodeTree);
        buildTree(nodeTrees);
    }
}

void MerkleTree::buildTree(std::vector<nodeTree *> *nodeTrees)
{
    float numberLeafnodeTrees = float(nodeTrees->size());
    int noLevels = 1;

    while ((numberLeafnodeTrees / 2) > 1)
    {
        noLevels++;
        numberLeafnodeTrees /= 2;
    }

    int totalLeafnodeTrees = pow(2, noLevels);

    while (nodeTrees->size() < totalLeafnodeTrees)
    {
        nodeTrees->push_back(new nodeTree("-1"));
    }

    while (nodeTrees->size() > 1)
    {
        nodeTree *leftnodeTree = (*nodeTrees)[0];
        nodeTree *rightnodeTree = (*nodeTrees)[1];
        nodeTrees->erase(nodeTrees->begin());
        nodeTrees->erase(nodeTrees->begin());
        string hash = sha256(leftnodeTree->value + rightnodeTree->value);
        nodeTree* parent = new nodeTree(hash);
        parent->left = leftnodeTree;
        parent->right = rightnodeTree;
        nodeTrees->push_back(parent);
    }

    this->root = *(nodeTrees->begin());
}


void MerkleTree::generateGraph(string &filename)
{
    std::string dotFile = filename + ".dot";
    std::string pngFile = filename + ".png";

    if (std::filesystem::exists(dotFile)) {
        std::filesystem::remove(dotFile);
    }
    if (std::filesystem::exists(pngFile)) {
        std::filesystem::remove(pngFile);
    }

    std::ofstream file(dotFile);
    file << "digraph G {" << std::endl;
    file << "node [shape=box];" << std::endl;
    writeNode(file, root);
    file << "}" << std::endl;
    file.close();

    std::string command = "dot -Tpng " + dotFile + " -o " + pngFile;
    int result = system(command.c_str());

    if (result != 0)
    {
        std::cout << "Ocurrió un error al generar la imagen.\n";
    }
    else
    {
        std::cout << "La imagen fue generada exitosamente.\n";
    }
}

// Función recursiva para escribir los nodos en el archivo DOT
void MerkleTree::writeNode(std::ofstream &file, nodeTree *node)
{
    if (node == nullptr)
        return;

    std::string label = node->value;

    if (node->left == nullptr && node->right == nullptr && !node->originalData.empty())
    {
        label = node->originalData;
    }

    std::string escapedLabel;
    for (char c : label)
    {
        if (c == '"' || c == '\\')
            escapedLabel += '\\';
        escapedLabel += c;
    }

    file << "\"" << node << "\"" << " [label=\"" << escapedLabel << "\"];" << std::endl;

    if (node->left)
    {
        file << "\"" << node << "\" -> \"" << node->left << "\";" << std::endl;
        writeNode(file, node->left);
    }
    if (node->right)
    {
        file << "\"" << node << "\" -> \"" << node->right << "\";" << std::endl;
        writeNode(file, node->right);
    }
}

std::string MerkleTree::getRootHash() {
    if (root) {
        return root->value;
    }
    return "";
}
