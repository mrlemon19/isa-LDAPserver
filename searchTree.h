// searchTree.h
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#ifndef SEARCH_TREE_H
#define SEARCH_TREE_H

#include <vector>
#include <iostream>

enum filterType_t{
    AND,
    OR,
    NOT,
    EQL,
    SUB
};

enum attributeType_t{
    UID,
    CN,
    MAIL
};

class searchTree
{
public:
    searchTree();
    searchTree(std::vector<char> filter, std::vector<attributeType_t> attributes);

    searchNode* root;
    std::vector<attributeType_t> attributes;
};

class searchNode
{
public:
    searchNode();
    searchNode(std::vector<char> filter);

    std::vector<char> attributes;
    std::vector<searchNode> children;
    filterType_t filterType;
};

#endif // SEARCH_TREE_H
