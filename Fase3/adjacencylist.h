#ifndef ADJACENCYLIST_H
#define ADJACENCYLIST_H
#include <iostream>
#include "node.h"
#include "qlist.h"

class AdjacencyList
{
private:


public:
    AdjacencyList();
    node* head;
    void insert(const QString& value);
    void createConnection(const QString& origin, const QString& destiny);
    void graph(const std::string &filename);
    void remove(const QString& value);
    bool contains(const QString& value) const;
    void print() const ;
    QList<QString> toList() const ;

};

#endif // ADJACENCYLIST_H
