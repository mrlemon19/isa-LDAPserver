// parsePacket.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "parsePacket.h"

void parsePacket(ByteStream bs)
{

    if (bs.readByte() != 0x30) return; // not a ldap packet

    bs.setLenght(static_cast<int>(static_cast<unsigned char>(bs.readByte()))); // set lenght of packet

    if (bs.readByte() != 0x02) return; // not a ldap packet

    switch (bs.readByte()) {
        case 0x01:
            bs.setMessageID(static_cast<int>(static_cast<unsigned char>(bs.readByte())));
            break;

        case 0x02:
            bs.setMessageID(static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 8 |
                            static_cast<int>(static_cast<unsigned char>(bs.readByte())));
            break;

        case 0x03:
            bs.setMessageID(static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 16 |
                            static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 8 |
                            static_cast<int>(static_cast<unsigned char>(bs.readByte())));
            break;

        case 0x04:
            bs.setMessageID(static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 24 |
                            static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 16 |
                            static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 8 |
                            static_cast<int>(static_cast<unsigned char>(bs.readByte())));
            break;
        
        default:

            // send error message not valid ldap message
            return;
            
    }

    // protocol op
    switch (bs.readByte()) {
        case 0x60:
            // bind request
            break;

        case 0x63:
            // search request
            break;

        case 0x42:
            // unbind request
            break;

        default:
            // send error message not supported operation
            return;
    }

}

