// byteStream.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "byteStream.h"

ByteStream::ByteStream(unsigned char* buffer) {
    this->buffer = buffer;
}

unsigned char ByteStream::readByte() {

    if (this->currentByte >= this->lenght) {
        unsigned char c = this->buffer[this->currentByte];
        this->currentByte++;
        return c;
    }
    else {
        return 0xff;
    }
}

void ByteStream::printBuffer() {
    std::cout << "Buffer: " << this->buffer << std::endl;
}

void ByteStream::setLenght(int lenght) {
    this->lenght = lenght;
}

void ByteStream::setMessageID(int messageID) {
    this->messageID = messageID;
}

int ByteStream::getMessageID() {
    return this->messageID;
}
