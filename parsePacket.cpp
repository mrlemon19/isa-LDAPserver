// parsePacket.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "parsePacket.h"

void parsePacket(ByteStream bs, int clientSocket, std::string DBfileName)
{

    // packet sender init
    packetSender* ps = new packetSender(clientSocket);

    if (bs.readByte() != 0x30){
        return; // not a ldap packet
    }
    //bs.setLenght(static_cast<int>(static_cast<unsigned char>(bs.readByte()))); // set lenght of packet

    bs.readByte(); // skip lenght

    if (bs.readByte() != 0x02) {
        return; // not a ldap packet
    }

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


    std::vector<unsigned char> response;

    // protocol op
    switch (bs.readByte()) {
        case 0x60:
            // bind request

            bs.readByte(); // skip lenght

            if (bs.readByte() != 0x02) return; // error in bind request

            if (bs.readByte() != 0x01) return; // error in bind request

            // TODO check if version is supported
            if (bs.readByte() != 0x03) return; // not suported version?

            if (bs.readByte() != 0x04) return; // error in bind request

            bs.readByte(); // skip byte

            // TODO chek rest of bind request
            //if (bs.readByte() != 0x80) return; // not a simple bind request

            //if (bs.readByte() != 0x00) return; // not a simple bind request

            // craft bind response backwards and sends it
            response = {0x00, 0x04, 0x00, 0x04, 0x00, 0x01, 0x0a};
            response.push_back(static_cast<unsigned char>(response.size()));
            response.push_back(0x61);

            ps->sendPacketLDAP(response, bs.getMessageID());

            break;

        case 0x63: {
            // search request

            bs.readByte(); // skip lenght
            
            if (bs.readByte() != 0x04) return; // error in search request

            bs.readByte(); // skip for now

            if (bs.readByte() != 0x0a) return; // error in search request

            if (bs.readByte() != 0x01) return; // error in search request

            bs.readByte(); // skip ldap scope

            if (bs.readByte() != 0x0a) return; // error in search request

            if (bs.readByte() != 0x01) return; // error in search request

            bs.readByte(); // skip ldap deref aliases

            if (bs.readByte() != 0x02) return; // error in search request

            //if (bs.readByte() != 0x01) return; // error in search request

            // sizelimit handle
            int sizeLimit;

            switch (bs.readByte()){
                case 0x01: {
                    sizeLimit = static_cast<int>(static_cast<unsigned char>(bs.readByte()));
                    break;
                }
                case 0x02: {
                    sizeLimit = static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 8 |
                                static_cast<int>(static_cast<unsigned char>(bs.readByte()));
                    break;
                }
                case 0x03: {
                    sizeLimit = static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 16 |
                                static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 8 |
                                static_cast<int>(static_cast<unsigned char>(bs.readByte()));
                    break;
                }
                case 0x04: {
                    sizeLimit = static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 24 |
                                static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 16 |
                                static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 8 |
                                static_cast<int>(static_cast<unsigned char>(bs.readByte()));
                    break;
                }
                default: {
                    return; // error in search request
                }
            }

            if (sizeLimit == 0) sizeLimit = 100;

            if (bs.readByte() != 0x02) return; // error in search request

            if (bs.readByte() != 0x01) return; // error in search request

            bs.readByte(); // skip time limit

            if (bs.readByte() != 0x01) return; // error in search request

            if (bs.readByte() != 0x01) return; // error in search request

            bs.readByte(); // skip types only

            // parsing filter

            
            std::vector<unsigned char> filter;
            std::vector<unsigned char> attributes;

            // TODO catch when there is no filter or attributes

            filter.push_back(bs.readByte());
            unsigned char lenght = bs.readByte();    // lenght of filter
            filter.push_back(lenght);
            // moving filter to vector
            for (int i = 0; i < static_cast<int>(static_cast<unsigned char>(lenght)); i++) {
                filter.push_back(bs.readByte());
            }

            // parsing attributes
            
            if (bs.readByte() != 0x30) return; // error in search request

            int attributesLenght = bs.readByte(); // lenght of attributes

            for (int i = 0; i < attributesLenght; i++) {
                attributes.push_back(bs.readByte());
            }

            searchTree filterTree = searchTree(filter, attributes, bs.getMessageID(), clientSocket, DBfileName, ps, sizeLimit);
            filterTree.search();

            // sends search result done
            response = {0x00, 0x04, 0x00, 0x04, 0x00, 0x01, 0x0a};
            response.push_back(static_cast<char>(response.size()));
            response.push_back(0x65);

            ps->sendPacketLDAP(response, bs.getMessageID());

            break;
        }

        case 0x42: {
            // unbind request

            if (bs.readByte() == 0x00) {
                close(clientSocket); // exit if unbind request
                return; 
            }

            break;
        }

        default: {
            // send error message not supported operation
            return;
        }
    }

    return;

}

