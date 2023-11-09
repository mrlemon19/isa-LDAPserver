// searchTree.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "searchTree.h"

searchTree::searchTree(std::vector<unsigned char> filter, std::vector<unsigned char> attributes, int messageID, int clientSocket, std::string DBfileName, packetSender* ps)
{

    this->root = new searchNode(filter);
    this->attributes = parseAttributes(attributes);
    this->messageID = messageID;
    this->clientSocket = clientSocket;
    this->maxResEnt = 100;
    this->DBfileName = DBfileName;
    this->ps = ps;

}

void searchTree::search()
{

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

            //std::cout << "match found" << std::endl;
            //std::cout << "name: " << entry.name << std::endl;
            //std::cout << "uid: " << entry.uid << std::endl;
            //std::cout << "mail: " << entry.mail << std::endl;

            // send entry to client
            std::vector<unsigned char> resEntry;

            // add entry to vector backwards
            // add requested attributes from entry
            if (std::find(this->attributes.begin(), this->attributes.end(), MAIL) != this->attributes.end()){
                // add mail if in requested attribtes list
                for (int i = entry.mail.size() - 1; i >= 0; i--){
                    resEntry.push_back(entry.mail[i]);
                }
                // push load lenght and type
                resEntry.push_back(entry.mail.size());
                resEntry.push_back(0x04);

                resEntry.push_back(entry.mail.size() + 2);
                resEntry.push_back(0x31);

                // push attribute type "mail"
                resEntry.push_back(0x6c);
                resEntry.push_back(0x69);
                resEntry.push_back(0x61);
                resEntry.push_back(0x6d);

                // push load lenght and type
                resEntry.push_back(0x04);
                resEntry.push_back(0x04);

                // push parsial attribute list lenght
                resEntry.push_back(entry.mail.size() + 10);
                resEntry.push_back(0x30);
            }
        
            if (std::find(this->attributes.begin(), this->attributes.end(), CN) != this->attributes.end()){
                // add cn if in requested attribtes list
                for (int i = entry.name.size() - 1; i >= 0; i--){
                    resEntry.push_back(entry.name[i]);
                }
                // push load lenght and type
                resEntry.push_back(entry.name.size());
                resEntry.push_back(0x04);

                resEntry.push_back(entry.name.size() + 2);
                resEntry.push_back(0x31);

                // push attribute type
                resEntry.push_back(0x6e);
                resEntry.push_back(0x63);

                // push load lenght and type
                resEntry.push_back(0x02);
                resEntry.push_back(0x04);

                // push parsial attribute list lenght
                resEntry.push_back(entry.name.size() + 8);
                resEntry.push_back(0x30);
            }

            if (std::find(this->attributes.begin(), this->attributes.end(), UID) != this->attributes.end()){
                // add uid if in requested attribtes list
                for (int i = entry.uid.size() - 1; i >= 0; i--){
                    resEntry.push_back(entry.uid[i]);
                }
                // push load lenght and type
                resEntry.push_back(entry.uid.size());
                resEntry.push_back(0x04);

                resEntry.push_back(entry.uid.size() + 2);
                resEntry.push_back(0x31);

                // push attribute type
                resEntry.push_back(0x64);
                resEntry.push_back(0x69);
                resEntry.push_back(0x75);

                // push load lenght and type
                resEntry.push_back(0x03);
                resEntry.push_back(0x04);

                // push parsial attribute list lenght
                resEntry.push_back(entry.uid.size() + 9);
                resEntry.push_back(0x30);
            }

            // push attribute list lenght and sing

            resEntry.push_back(resEntry.size());
            resEntry.push_back(0x30);

            // push object name and lenght
            for (int i = entry.uid.size() - 1; i >= 0; i--){
                resEntry.push_back(entry.uid[i]);
            }

            resEntry.push_back(entry.uid.size());
            resEntry.push_back(0x04);

            resEntry.push_back(resEntry.size());
            resEntry.push_back(0x64);

            // send the packet to the client
            this->ps->sendPacketLDAP(resEntry, this->messageID);

        }

    }

}

std::vector<attributeType_t> searchTree::parseAttributes(std::vector<unsigned char> attributes)
{
    // parse attributes
    std::vector<attributeType_t> parsedAttributes;
    size_t lenght = attributes.size();
    size_t i = 0;

    while (i < lenght){

        std::string attribute;
        
        if (attributes[i] == 0x04){
            // attribute type is specified
            i++;
            int attributeLenght = attributes[i];
            i++;
            for (int j = 0; j < attributeLenght; j++){
                attribute += attributes[i];
                i++;
            }
        }

        else{
            // no attributes specified, return default attributes
            parsedAttributes.push_back(UID);
            parsedAttributes.push_back(CN);
            parsedAttributes.push_back(MAIL);
        }

        if (attribute == "uid"){
            parsedAttributes.push_back(UID);
        }
        else if (attribute == "cn"){
            parsedAttributes.push_back(CN);
        }
        else if (attribute == "mail"){
            parsedAttributes.push_back(MAIL);
        }
        else{
            std::cout << "Invalid attribute type" << std::endl;
            return std::vector<attributeType_t> {UID, CN, MAIL};
        }
    }

    if (parsedAttributes.size() == 0){
        // no attributes specified, return default attributes
        parsedAttributes.push_back(UID);
        parsedAttributes.push_back(CN);
        parsedAttributes.push_back(MAIL);
    }

    return parsedAttributes;

}

searchNode::searchNode(std::vector<unsigned char> filter)
{

    this->filter = filter;
    this->filterIndex = 0;
    this->parseFilter();

}

void searchNode::parseFilter()
{

    // parse filter
    unsigned char filterType = this->readChar();
    this->filterLenght = this->readChar();

    switch (filterType){

        case 0xa0:{
            this->filterType = AND;

            // sepatares filters from inside AND filter and creates searchNodes from them
            std::vector<std::vector<unsigned char>> filters = this->separateFilter();

            for (std::vector<unsigned char> filter : filters){
                searchNode* node = new searchNode(filter);
                this->children.push_back(node);
            }

            break;
        }

        case 0xa1:{
            this->filterType = OR;

            // sepatares filters from inside OR filter and creates searchNodes from them
            std::vector<std::vector<unsigned char>> filters = this->separateFilter();

            for (std::vector<unsigned char> filter : filters){
                searchNode* node = new searchNode(filter);
                this->children.push_back(node);
            }

            break;
        }

        case 0xa2:{
            this->filterType = NOT;

            std::vector<unsigned char> innerFilter;

            // copy filter characters into vector
            for (int i = 0; i < this->filterLenght; i++){
                innerFilter.push_back(this->readChar());
            }

            // create searchNode from inner filter
            searchNode* node = new searchNode(innerFilter);
            this->children.push_back(node);

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
                this->regexPattern += this->readChar();
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
}

bool searchNode::evaluate(ResultEntry* entry)
{   


    if (this->filterType == EQL){
        // equalityMatch

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

    else if (this->filterType == AND){
        // and

        for (searchNode* node : this->children){
            // if any of the children returns false, return false else return true
            if (!node->evaluate(entry)){
                return false;
            }
        }

        return true;

    }

    else if (this->filterType == OR){
        // or

        for (searchNode* node : this->children){
            // if any of the children returns true, return true else return false
            if (node->evaluate(entry)){
                return true;
            }
        }

        return false;

    }

    else if (this->filterType == NOT){
        // not

        return !this->children[0]->evaluate(entry);

    }

    else{
        std::cout << "Invalid filter type" << std::endl;
        return false;
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
    // remove \r from mail
    this->mail = this->mail.substr(0, this->mail.size() - 1);

}
