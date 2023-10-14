// byteStream.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "byteStream.h"

ByteStream::ByteStream(char* buffer) {
    this->buffer = buffer;
}

char ByteStream::readByte() {
    char c = this->buffer[this->currentByte];
    this->currentByte++;
    return c;
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
