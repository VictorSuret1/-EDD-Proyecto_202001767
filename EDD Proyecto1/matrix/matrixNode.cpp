#include "./headers/node.h"

MatrixNode::MatrixNode(int col, int row, const std::string& value){
  this->col = col;
  this->row = row;
  this->value = value;
  up = nullptr;
  bottom = nullptr;
  right = nullptr;
  left = nullptr;
}