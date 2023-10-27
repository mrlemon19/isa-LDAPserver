// searchTree.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "searchTree.h"

searchTree::searchTree(std::vector<unsigned char> filter, std::vector<attributeType_t> attributes)
{

    std::cout << "searchTree is being initialized" << std::endl;
    searchNode node = searchNode(filter);
    this->root = &node;
    this->attributes = attributes;
    std::cout << "searchTree initialized" << std::endl;

}

searchNode::searchNode(std::vector<unsigned char> filter)
{

    this->filter = filter;
    this->filterIndex = 0;

    // parse filter
    unsigned char filterType = this->readChar();

    switch (filterType){

        case 0xa0:{
            this->filterType = AND;
            break;
        }

        case 0xa1:{
            this->filterType = OR;
            break;
        }

        case 0xa2:{
            this->filterType = NOT;
            break;
        }

        case 0xa3:{
            this->filterType = EQL;
            break;
        }

        case 0xa4:{
            this->filterType = SUB;
            break;
        }

        default:{
            std::cout << "Invalid filter type" << std::endl;
            break;
        }

    }

    std::cout << "filter " << this->filterType << " initialized" << std::endl;

}

unsigned char searchNode::readChar()
{

    return this->filter[this->filterIndex++];

}
