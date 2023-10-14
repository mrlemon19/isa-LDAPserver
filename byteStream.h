// byteStream.h
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#pragma once

#include <iostream>

class ByteStream
{
    public:
    char* buffer;
    int currentByte = 0;

    ByteStream(char* buffer);
    char readByte();
    void printBuffer();
};
