#include "../../simpleList/headers/simpleList.h"
#include <unordered_map>
#include <string>

class Matrix {
private:
    SimpleList *colHeader;
    SimpleList *rowHeader;
    std::unordered_map<std::string, int> nombreAIndice;
    int indiceActual = 1;

    int obtenerIndice(const std::string& nombre);

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
    string obtenerNombrePorIndice(int indice);
public:
    Matrix();
    ~Matrix();

    void insertarAmistad(const std::string& nombre1, const std::string& nombre2);
    void insert(int col, int row, const std::string& value); // Aquí se corrige para aceptar std::string
    void create_dot();
};
