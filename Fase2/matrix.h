#include "simplelist.h"
#include <string>
#include <unordered_map>

#ifndef MATRIX_H
#define MATRIX_H

class Matrix {
private:

    std::unordered_map<std::string, int> nombreAIndice;
    int indiceActual = 1;

    void move_lr_pointers(ListNode *rowNode, MatrixNode *newMatrixNode);
    bool moveAt_first_lr_pointers(ListNode *rowNode, MatrixNode *newMatrixNode);
    MatrixNode* moveAt_middle_lr_pointers(ListNode *rowNode, MatrixNode *newMatrixNode);
    void moveAt_end_lr_pointers(MatrixNode *lastNode, MatrixNode *newMatrixNode);

    void move_ub_pointers(ListNode *colNode, MatrixNode *newMatrixNode);
    bool moveAt_first_ub_pointers(ListNode *colNode, MatrixNode *newMatrixNode);
    MatrixNode* moveAt_middle_ub_pointers(ListNode *colNode, MatrixNode *newMatrixNode);
    void moveAt_end_ub_pointers(MatrixNode *lastNode, MatrixNode *newMatrixNode);

    string get_content();

    void align_col_nodes(stringstream &createNodes, stringstream &align, stringstream &linkNodes);
    string get_address_memory(MatrixNode* node);
    void write_dot(const string &code);



public:
    SimpleList *colHeader;
    SimpleList *rowHeader;

    string obtenerNombrePorIndice(int indice);

    string get_content_amigos(const std::string& correoLogeado);
    void align_col_nodes_amigos(std::stringstream &createNodes, std::stringstream &align, std::stringstream &linkNodes, const std::string& correoLogeado);


    int obtenerIndice(const std::string& correo);
    bool sonAmigos(const std::string& correo1, const std::string& correo2);
    SimpleList* getRowHeader() const {
        return rowHeader;
    }

    std::string getNombrePorIndice(int indice) {
        return obtenerNombrePorIndice(indice);
    }


    Matrix();
    ~Matrix();

    void insertarAmistad(const std::string& nombre1, const std::string& nombre2);
    void insert(int col, int row, const std::string& value); // Aquí se corrige para aceptar std::string
    void create_dot();
    void create_dot2(const std::string& correoLogeado);

};
#endif // MATRIX_H
