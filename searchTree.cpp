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
    this->filterLenght = filter.size();

    // parse filter
    unsigned char filterType = this->readChar();
    this->readChar();   // skip lenght

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

            // parse attribute type

            if (this->readChar() != 0x04){
                std::cout << "Invalid attribute type" << std::endl;
                break;
            }

            int lenght = this->readChar();
            std::string attributeType;

            for (int i = 0; i < lenght; i++){
                attributeType += this->readChar();
            }

            if (attributeType == "uid"){
                this->attributeType = UID;
            }
            else if (attributeType == "cn"){
                this->attributeType = CN;
            }
            else if (attributeType == "mail"){
                this->attributeType = MAIL;
            }
            else{
                std::cout << "Invalid attribute type" << std::endl;
                break;
            }

            // parse attribute value
            if (this->readChar() != 0x04){
                std::cout << "Invalid attribute value" << std::endl;
                break;
            }

            lenght = this->readChar();
            for (int i = 0; i < lenght; i++){
                this->preStr += this->readChar();
            }

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

attributeType_t getAttributeType(std::string attributeType)
{

    if (attributeType == "uid"){
        return UID;
    }
    else if (attributeType == "cn"){
        return CN;
    }
    else if (attributeType == "mail"){
        return MAIL;
    }
    else{
        std::cout << "Invalid attribute type" << std::endl;
        return UID;
    }

}
