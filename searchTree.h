// searchTree.h
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#ifndef SEARCH_TREE_H
#define SEARCH_TREE_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <regex>

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

class ResultEntry
{

    public:
        ResultEntry(std::string entry);
        
        std::string name;
        std::string uid;
        std::string mail;

};

class searchNode;

class searchTree
{
public:
    searchTree();
    searchTree(std::vector<unsigned char> filter, std::vector<attributeType_t> attributes, int messageID, int clientSocket, std::string DBfileName);
    void search();

    searchNode* root;
    std::vector<attributeType_t> attributes;
    int messageID;
    int clientSocket;
    int maxResEnt;
    std::string DBfileName;
};

class searchNode
{
public:
    searchNode();
    searchNode(std::vector<unsigned char> filter);
    void parseFilter();
    bool evaluate(ResultEntry* entry);
    unsigned char readChar();
    attributeType_t getAttributeType(std::string attributeType);
    std::vector<std::vector<unsigned char>> separateFilter();

    std::vector<unsigned char> filter;
    int filterIndex;
    int filterLenght;
    std::vector<searchNode> children;
    filterType_t filterType;

    // for eql or sub filter
    std::string regexPattern;
    attributeType_t attributeType;
};

#endif // SEARCH_TREE_H
