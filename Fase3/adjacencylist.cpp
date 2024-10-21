#include "adjacencylist.h"
#include <QDebug>

AdjacencyList::AdjacencyList() {
    head = nullptr;
}

// Insertar un nuevo nodo
void AdjacencyList::insert(const QString& value) {
    node* newnode = new node(value);
    if (!head) {
        head = newnode;
    } else if (value < head->value) {
        newnode->next = head;
        head = newnode;
    } else {
        auto aux = head;
        while (aux->next && aux->next->value < value) {
            aux = aux->next;
        }
        newnode->next = aux->next;
        aux->next = newnode;
    }
}

// Crear una conexión entre dos nodos
void AdjacencyList::createConnection(const QString& origin, const QString& destiny) {
    node* current = head;
    while (current) {
        if (current->value == origin) {
            current->insertNeighbor(destiny);
            break;
        }
        current = current->next;
    }
}

void AdjacencyList::remove(const QString& value) {
    node* current = head;
    node* prev = nullptr;

    while (current) {
        if (current->value == value) {
            if (prev) {
                prev->next = current->next;
            } else {
                head = current->next;
            }
            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}

bool AdjacencyList::contains(const QString& value) const {
    node* current = head;
    while (current) {
        if (current->value == value) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void AdjacencyList::print() const {
    node* current = head;
    while (current) {
        qDebug() << current->value;
        current = current->next;
    }
}

QList<QString> AdjacencyList::toList() const {
    QList<QString> list;
    node* current = head;
    while (current) {
        list.append(current->value);
        current = current->next;
    }
    return list;
}






