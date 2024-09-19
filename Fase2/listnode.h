#include "matrixnode.h"

#ifndef LISTNODE_H
#define LISTNODE_H

using namespace std;

class ListNode{
private:
    /* data */
public:
    // CONSTRUCTORS
    ListNode();   // Default constructor
    ListNode(int position);  //Parameterised Constructor
    // VARIABLES
    int position;
    ListNode *next;
    MatrixNode *access;


};

#endif // LISTNODE_H
