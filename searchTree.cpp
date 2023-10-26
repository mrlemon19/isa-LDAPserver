// searchTree.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "searchTree.h"

searchTree::searchTree(std::vector<char> filter, std::vector<attributeType_t> attributes)
{

    searchNode node = searchNode(filter);
    this->root = &node;
    this->attributes = attributes;

}

searchNode::searchNode(std::vector<char> filter)
{



}
