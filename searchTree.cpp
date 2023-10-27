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

            this->attributeType = this->getAttributeType(attributeType);

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

            this->attributeType = this->getAttributeType(attributeType);

            // parse attribute value

            if (this->readChar() != 0x30){
                std::cout << "Invalid attribute value" << std::endl;
                break;
            }

            this->readChar();   // skip lenght

            unsigned char subFilterType = this->readChar();

            if (subFilterType == 0x81){
                std::cout << "now parsing prefix" << std::endl;
                // loding prefix substring if there is one
                lenght = this->readChar();
                for (int i = 0; i < lenght; i++){
                    this->preStr += this->readChar();
                }
                subFilterType = this->readChar();
            }
            
            while (subFilterType == 0x81)
            {
                std::cout << "now parsing prefix" << std::endl;
                // loads sub strings while there are sone
                lenght = this->readChar();
                std::string subStr;
                for (int i = 0; i < lenght; i++){
                    subStr += this->readChar();
                }

                this->subStrings.push_back(subStr);
                subFilterType = this->readChar();
            }
            
            if (subFilterType == 0x82){
                std::cout << "now parsing prefix" << std::endl;
                // loading postfix substring if there is one
                lenght = this->readChar();
                for (int i = 0; i < lenght; i++){
                    this->postStr += this->readChar();
                }
            }


            break;
        }

        default:{
            std::cout << "Invalid filter type" << std::endl;
            break;
        }

    }

    std::cout << "filter " << this->filterType << " initialized" << std::endl;
    std::string substrings;
    for (std::string subStr : this->subStrings){
        substrings += subStr + "*";
    }
    std::cout << "filter: " << this->preStr << "*" << substrings << this->postStr << std::endl;
}

unsigned char searchNode::readChar()
{

    return this->filter[this->filterIndex++];

}

attributeType_t searchNode::getAttributeType(std::string attributeType)
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
