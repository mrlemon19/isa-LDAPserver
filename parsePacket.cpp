// parsePacket.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "parsePacket.h"

void parsePacket(ByteStream bs, int clientSocket)
{

    std::cout << "parsing packet: " << bs.buffer << std::endl;

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

    std::vector<char> response;

    // protocol op
    switch (bs.readByte()) {
        case 0x60:
            // bind request

            bs.readByte(); // skip lenght

            if (bs.readByte() != 0x02) return; // error in bind request

            if (bs.readByte() != 0x01) return; // error in bind request

            if (bs.readByte() != 0x02) return; // not suported version?

            if (bs.readByte() != 0x04) return; // error in bind request

            bs.readByte(); // skip byte

            if (bs.readByte() != 0x80) return; // not a simple bind request

            if (bs.readByte() != 0x00) return; // not a simple bind request

            // craft bind response backwards
            response = {0x00, 0x04, 0x00, 0x04, 0x00, 0x0, 0x0a};
            response.push_back(static_cast<char>(response.size() + '0'));
            response.push_back(0x61);
            // TODO function to craft the rest of the packet


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

    return;

}

