
#include <string>

class MatrixNode {
public:
    MatrixNode(int col, int row, const std::string& value);

    int col;
    int row;
    std::string value;
    MatrixNode *up;
    MatrixNode *bottom;
    MatrixNode *right;
    MatrixNode *left;
};