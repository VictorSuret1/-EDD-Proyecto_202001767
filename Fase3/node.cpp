#include "node.h"

node::node(const QString& val) {
    value = val;
    next = nullptr;
    neighbors = nullptr;
}

// Insertar vecino a este nodo
void node::insertNeighbor(const QString& destiny) {
    Neighbor* newNeighbor = new Neighbor(destiny);
    if (!neighbors) {
        neighbors = newNeighbor;
    } else {
        Neighbor* current = neighbors;
        while (current) {
            if (current->value == destiny) return; // Evita duplicados
            if (!current->next) {
                current->next = newNeighbor;
                return;
            }
            current = current->next;
        }
    }
}

// Generar el archivo de vecinos de este nodo
void node::graphNeighbors(std::ofstream& out) {
    Neighbor* aux = neighbors;
    std::stringstream origin_name;
    origin_name << "\"Nodo " << value.toStdString() << "\""; // Convertir QString a std::string
    while (aux) {
        std::stringstream destiny_name;
        destiny_name << "\"Nodo " << aux->value.toStdString() << "\""; // Convertir QString a std::string
        out << origin_name.str() << " -> " << destiny_name.str() << " [dir = both];\n";
        aux = aux->next;
    }
}


