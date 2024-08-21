#include <fstream>
#include <sstream>
#include <memory>
#include "./headers/Matrix.h"

Matrix::Matrix() {
    rowHeader = new SimpleList(0);
    colHeader = new SimpleList(1);
}

int Matrix::obtenerIndice(const std::string& correo) {
    if (nombreAIndice.find(correo) == nombreAIndice.end()) {
        nombreAIndice[correo] = indiceActual++;
    }
    return nombreAIndice[correo];
}

void Matrix::insertarAmistad(const std::string& correo1, const std::string& correo2) {
    int col = obtenerIndice(correo1);
    int row = obtenerIndice(correo2);

    insert(col, row, "Amistad");
    insert(row, col, "Amistad");
}

void Matrix::insert(int col, int row, const std::string& value) {
    ListNode *colNode = colHeader->insert(col);
    ListNode *rowNode = rowHeader->insert(row);

    MatrixNode *newMatrixNode = new MatrixNode(col, row, value);

    move_lr_pointers(rowNode, newMatrixNode);
    move_ub_pointers(colNode, newMatrixNode);
}

void Matrix::move_lr_pointers(ListNode *rowNode, MatrixNode *newMatrixNode) {
    bool isFirst = false;
    MatrixNode *lastNode = nullptr;

    isFirst = moveAt_first_lr_pointers(rowNode, newMatrixNode);
    if (isFirst) return;
    lastNode = moveAt_middle_lr_pointers(rowNode, newMatrixNode);
    if (!lastNode) return;

    moveAt_end_lr_pointers(lastNode, newMatrixNode);
}

bool Matrix::moveAt_first_lr_pointers(ListNode *rowNode, MatrixNode *newNode) {
    MatrixNode *current = rowNode->access;

    if (current == nullptr) {
        rowNode->access = newNode;
        return true;
    } else if (newNode->col < current->col) {
        newNode->right = rowNode->access;
        rowNode->access->left = newNode;
        rowNode->access = newNode;
        return true;
    }

    return false;
}

MatrixNode* Matrix::moveAt_middle_lr_pointers(ListNode *rowNode, MatrixNode *newNode) {
    MatrixNode *current = rowNode->access;
    bool isMiddle = false;

    while (current->right != nullptr) {
        isMiddle = current->col < newNode->col && newNode->col < current->right->col;
        if (isMiddle) {
            newNode->left = current;
            newNode->right = current->right;
            current->right->left = newNode;
            current->right = newNode;
            return nullptr;
        }
        current = current->right;
    }
    return current;
}

void Matrix::moveAt_end_lr_pointers(MatrixNode *lastNode, MatrixNode *newNode) {
    lastNode->right = newNode;
    newNode->left = lastNode;
}

void Matrix::move_ub_pointers(ListNode *colNode, MatrixNode *newMatrixNode) {
    bool isFirst = false;
    MatrixNode *lastNode = nullptr;

    isFirst = moveAt_first_ub_pointers(colNode, newMatrixNode);
    if (isFirst) return;
    lastNode = moveAt_middle_ub_pointers(colNode, newMatrixNode);
    if (!lastNode) return;

    moveAt_end_ub_pointers(lastNode, newMatrixNode);
}

bool Matrix::moveAt_first_ub_pointers(ListNode *colNode, MatrixNode *newNode) {
    MatrixNode *current = colNode->access;

    if (current == nullptr) {
        colNode->access = newNode;
        return true;
    } else if (newNode->row < current->row) {
        newNode->bottom = colNode->access;
        colNode->access->up = newNode;
        colNode->access = newNode;
        return true;
    }

    return false;
}

MatrixNode* Matrix::moveAt_middle_ub_pointers(ListNode *colNode, MatrixNode *newNode) {
    MatrixNode *current = colNode->access;
    bool isMiddle = false;

    while (current->bottom != nullptr) {
        isMiddle = current->row < newNode->row && newNode->row < current->bottom->row;
        if (isMiddle) {
            newNode->up = current;
            newNode->bottom = current->bottom;
            current->bottom->up = newNode;
            current->bottom = newNode;
            return nullptr;
        }
        current = current->bottom;
    }
    return current;
}

void Matrix::moveAt_end_ub_pointers(MatrixNode *lastNode, MatrixNode *newNode) {
    lastNode->bottom = newNode;
    newNode->up = lastNode;
}

void Matrix::create_dot() {
    string code = "digraph G{\n";
    code += "  node[shape=box];\n";
    code += "  MTX[ label = \"Matrix\", style = filled, fillcolor = firebrick1, group = 0 ];\n";

    code += get_content();
    code += "}\n";

    write_dot(code);
}

string Matrix::get_content() {
    stringstream content, alignCols, alignRows, createRowNodes, createColNodes, createMtxNodes, linkNodes;
    ListNode *row_node = rowHeader->head;
    MatrixNode *current_mtx_node;
    string address, address2;

    align_col_nodes(createColNodes, alignCols, linkNodes);

    // link first row node to matrix node
    std::string nombre_row = obtenerNombrePorIndice(row_node->position);
    linkNodes << "  \"MTX\" -> \"r" << nombre_row << "\";\n";

    while (row_node) {
        nombre_row = obtenerNombrePorIndice(row_node->position);

        // create row node with user name
        createRowNodes << "  \"r" << nombre_row << "\" [label = \"" << nombre_row
                       << "\"  style = filled, fillcolor = bisque1, group = 0 ];\n";

        // align row node
        alignRows << "  { rank = same; \"r" << nombre_row << "\";";

        // link next row node
        if (row_node->next) {
            std::string nombre_next = obtenerNombrePorIndice(row_node->next->position);
            linkNodes << "  \"r" << nombre_row << "\" -> \"r" << nombre_next << "\";\n";
        }

        // link row node to first matrix node
        current_mtx_node = row_node->access;
        address = get_address_memory(current_mtx_node);
        linkNodes << "  \"r" << nombre_row << "\" -> \"" << address << "\";\n";

        while (current_mtx_node) {
            // create matrix node
            address = get_address_memory(current_mtx_node);
            createMtxNodes << "  \"" << address << "\" [label = \"" << current_mtx_node->value
                           << "\" group = " << current_mtx_node->col << "];\n";

            // align matrix nodes
            alignRows << "\"" << address << "\";";

            // link matrix nodes RIGHT, LEFT, DOWN, UP
            if (current_mtx_node->right) {
                address2 = get_address_memory(current_mtx_node->right);
                linkNodes << "  \"" << address << "\" -> \"" << address2 << "\";\n";
            }
            if (current_mtx_node->left) {
                address2 = get_address_memory(current_mtx_node->left);
                linkNodes << "  \"" << address << "\" -> \"" << address2 << "\";\n";
            }
            if (current_mtx_node->bottom) {
                address2 = get_address_memory(current_mtx_node->bottom);
                linkNodes << "  \"" << address << "\" -> \"" << address2 << "\";\n";
            }
            if (current_mtx_node->up) {
                address2 = get_address_memory(current_mtx_node->up);
                linkNodes << "  \"" << address << "\" -> \"" << address2 << "\";\n";
            } else {
                std::string nombre_col = obtenerNombrePorIndice(current_mtx_node->col);
                linkNodes << "  \"c" << nombre_col << "\" -> \"" << address << "\";\n";
            }

            current_mtx_node = current_mtx_node->right;
        }

        alignRows << "};\n";
        row_node = row_node->next;
    }

    content << createColNodes.str() << createRowNodes.str() << alignCols.str()
            << alignRows.str() << createMtxNodes.str() << linkNodes.str();

    return content.str();
}

void Matrix::align_col_nodes(stringstream &createNodes, stringstream &align, stringstream &linkNodes) {
    ListNode *col_hdr_node = colHeader->head;

    align << "  { rank = same; \"MTX\";";

    while (col_hdr_node) {
        std::string nombre = obtenerNombrePorIndice(col_hdr_node->position);

        // create column node with user name
        createNodes << "  \"c" << nombre << "\" [label = \"" << nombre
                    << "\"  style = filled, fillcolor = bisque1, group = " << col_hdr_node->position << " ];\n";

        // align column node
        align << "\"c" << nombre << "\";";

        // link next and prev nodes
        if (col_hdr_node->next) {
            std::string nombre_next = obtenerNombrePorIndice(col_hdr_node->next->position);
            linkNodes << "  \"c" << nombre << "\" -> \"c" << nombre_next << "\";\n";
        }

        col_hdr_node = col_hdr_node->next;
    }

    align << "};\n";
}

void Matrix::write_dot(const string &code) {
    ofstream file("graph.dot");
    if (file.is_open()) {
        file << code;
        file.close();
    } else {
        cerr << "Unable to open file";
    }
}

string Matrix::get_address_memory(MatrixNode* node) {
    ostringstream oss;
    oss << reinterpret_cast<uintptr_t>(node);
    return oss.str();
}

bool Matrix::sonAmigos(const std::string& correo1, const std::string& correo2) {
    int col = obtenerIndice(correo1);
    int row = obtenerIndice(correo2);

    MatrixNode* nodo = rowHeader->head->access;
    while (nodo != nullptr) {
        if (nodo->col == col && nodo->row == row) {
            return true;
        }
        nodo = nodo->right;
    }

    return false;
}

string Matrix::obtenerNombrePorIndice(int indice) {
    for (const auto& par : nombreAIndice) {
        if (par.second == indice) {
            return par.first;
        }
    }
    return "";
}

Matrix::~Matrix() {
    delete rowHeader;
    delete colHeader;
}
