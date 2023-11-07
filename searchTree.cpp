// searchTree.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "searchTree.h"

searchTree::searchTree(std::vector<unsigned char> filter, std::vector<attributeType_t> attributes, int messageID, int clientSocket, std::string DBfileName)
{

    std::cout << "searchTree is being initialized" << std::endl;
    this->root = new searchNode(filter);
    this->attributes = attributes;
    this->messageID = messageID;
    this->clientSocket = clientSocket;
    this->maxResEnt = 100;
    this->DBfileName = DBfileName;
    std::cout << "searchTree initialized" << std::endl;

}

void searchTree::search()
{

    std::cout << "searchTree is being searched" << std::endl;

    // open DB file
    std::ifstream DBStream(this->DBfileName);

    // check if DB file is open
    if (!DBStream.is_open()){
        std::cout << "DB file could not be opened" << std::endl;
        return;
    }

    // read DB file line by line
    std::string line;
    while (std::getline(DBStream, line)){

        // parse line
        ResultEntry entry = ResultEntry(line);

        // check if entry matches filter
        if (this->root->evaluate(&entry)){

            std::cout << "match found" << std::endl;
            std::cout << "name: " << entry.name << std::endl;
            std::cout << "uid: " << entry.uid << std::endl;
            std::cout << "mail: " << entry.mail << std::endl;

            // TODO create vector of the entry and send it
        
        }

    }

}

searchNode::searchNode(std::vector<unsigned char> filter)
{

    std::cout << "search node is being initialized" << std::endl;
    this->filter = filter;
    this->filterIndex = 0;
    this->parseFilter();
    std::cout << "search node initialized" << std::endl;

}

void searchNode::parseFilter()
{

    // parse filter
    unsigned char filterType = this->readChar();
    this->filterLenght = this->readChar();

    switch (filterType){

        case 0xa0:{
            this->filterType = AND;

            std::cout << "AND filter" << std::endl;

            // sepatares filters from inside AND filter and creates searchNodes from them
            std::vector<std::vector<unsigned char>> filters = this->separateFilter();

            for (std::vector<unsigned char> filter : filters){
                searchNode node = searchNode(filter);
                this->children.push_back(node);
            }

            break;
        }

        case 0xa1:{
            this->filterType = OR;

            std::cout << "OR filter" << std::endl;

            // sepatares filters from inside OR filter and creates searchNodes from them
            std::vector<std::vector<unsigned char>> filters = this->separateFilter();

            for (std::vector<unsigned char> filter : filters){
                searchNode node = searchNode(filter);
                this->children.push_back(node);
            }

            break;
        }

        case 0xa2:{
            this->filterType = NOT;

            std::cout << "NOT filter" << std::endl;

            std::vector<unsigned char> innerFilter;

            // copy filter characters into vector
            for (int i = 0; i < this->filterLenght; i++){
                innerFilter.push_back(this->readChar());
            }

            // create searchNode from inner filter
            searchNode node = searchNode(innerFilter);

            break;
        }

        case 0xa3:{
            this->filterType = EQL;

            // parse attribute type

            std::cout << "EQL filter" << std::endl;

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
                this->regexPattern += this->readChar();
            }

            break;
        }

        case 0xa4:{
            this->filterType = SUB;

            // parse attribute type

            std::cout << "SUB filter" << std::endl;

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

            std::string preStr;
            std::vector<std::string> subStrings;
            std::string postStr;

            if (subFilterType == 0x80){
                // loding prefix substring if there is one
                lenght = this->readChar();
                preStr = "^";
                for (int i = 0; i < lenght; i++){
                    preStr += this->readChar();
                }
                subFilterType = this->readChar();
            }
            
            while (subFilterType == 0x81)
            {
                // loads sub strings while there are sone
                lenght = this->readChar();
                std::string subStr = ".*";
                for (int i = 0; i < lenght; i++){
                    subStr += this->readChar();
                }

                subStrings.push_back(subStr);
                subFilterType = this->readChar();
            }
            
            if (subFilterType == 0x82){
                // loading postfix substring if there is one
                lenght = this->readChar();
                postStr = ".*";
                for (int i = 0; i < lenght; i++){
                    postStr += this->readChar();
                }
                postStr += "$";
            }

            // creating regex pattern
            std::string pattern = preStr;

            for (std::string subStr : subStrings){
                pattern += subStr;
            }

            pattern += postStr;

            this->regexPattern = pattern;

            break;
        }

        default:{
            std::cout << "Invalid filter type" << std::endl;
            break;
        }

    }

    std::cout << "filter " << this->filterType << " initialized" << std::endl;

}

bool searchNode::evaluate(ResultEntry* entry)
{   

    std::cout << "evaluating filter " << this->filterType << std::endl;

    if (this->filterType == EQL){
        // equalityMatch

        std::cout << "evaluating " << this->regexPattern << "and " << entry->uid << std::endl;

        switch (this->attributeType){
            case UID:{
                return this->regexPattern == entry->uid;
                break;
            }
            case CN:{
                return this->regexPattern == entry->name;
                break;
            }
            case MAIL:{
                return this->regexPattern == entry->mail;
                break;
            }
            default:{
                std::cout << "Invalid attribute type" << std::endl;
                return false;
                break;
            }
        }

    }

    else if (this->filterType == SUB){
        // substring

        std::cout << "evaluating " << this->regexPattern << " and " << entry->uid << std::endl;

        switch (this->attributeType){
            case UID:{
                return std::regex_search(entry->uid, std::regex(this->regexPattern));
                break;
            }
            case CN:{
                return std::regex_search(entry->name, std::regex(this->regexPattern));
                break;
            }
            case MAIL:{
                return std::regex_search(entry->mail, std::regex(this->regexPattern));
                break;
            }
            default:{
                std::cout << "Invalid attribute type" << std::endl;
                return false;
                break;
            }
        }

    }

    return false;

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

std::vector<std::vector<unsigned char>> searchNode::separateFilter()
{

    std::vector<std::vector<unsigned char>> filters;
    
    //this->readChar();   // skip filter type
    //this->readChar();   // skip filter lenght

    unsigned char filterType = this->readChar();

    while (this->filterIndex < this->filterLenght){

        if (filterType == 0xa0 || filterType == 0xa1 || filterType == 0xa2 || filterType == 0xa3 || filterType == 0xa4){
            // filter starts
            int lenght = this->readChar();
            std::vector<unsigned char> filter;

            std::cout << "separating filter " << this->filterType << std::endl;

            // copy filter characters into vector
            filter.push_back(filterType);
            filter.push_back(lenght);
            for (int i = 0; i < lenght; i++){
                filter.push_back(this->readChar());
            }

            // push filter into vector
            filters.push_back(filter);
            filterType = this->readChar();
        }
    }

    return filters;

}

ResultEntry::ResultEntry(std::string entry)
{

    // parse entry
    size_t firstSemicolon = entry.find(';');
    size_t secondSemicolon = entry.find(';', firstSemicolon + 1);

    this->name = entry.substr(0, firstSemicolon);
    this->uid = entry.substr(firstSemicolon + 1, secondSemicolon - firstSemicolon - 1);
    this->mail = entry.substr(secondSemicolon + 1);

}
