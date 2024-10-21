#include "listnode.h"

ListNode::ListNode(){
    next = nullptr;
    position = -1;
    access = nullptr;
}
// Parameterised constructor
ListNode::ListNode(int position){
    this->position = position;
    this->next = nullptr;
    this->access = nullptr;
}
