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

class searchNode;

class searchTree
{
public:
    searchTree();
    searchTree(std::vector<unsigned char> filter, std::vector<attributeType_t> attributes);

    searchNode* root;
    std::vector<attributeType_t> attributes;
};

class searchNode
{
public:
    searchNode();
    searchNode(std::vector<unsigned char> filter);
    unsigned char readChar();
    attributeType_t getAttributeType(std::string attributeType);

    std::vector<unsigned char> filter;
    int filterIndex;
    int filterLenght;
    std::vector<searchNode> children;
    filterType_t filterType;

    // for eql or sub filter
    std::string preStr;
    std::string postStr;
    std::vector<std::string> subStrings;
    attributeType_t attributeType;
};

#endif // SEARCH_TREE_H
