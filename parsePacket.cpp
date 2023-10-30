// parsePacket.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "parsePacket.h"

void sendPacketLDAP(std::vector<char> response, int clientSocket, int messageID)
{
    // crafts LDAP packet backwards
    response.push_back(static_cast<char>(messageID));
    response.push_back(0x01);
    response.push_back(0x02);
    response.push_back(static_cast<char>(response.size()));
    response.push_back(0x30);

    std::reverse(response.begin(), response.end());

    std::cout << "sending response" << std::endl;
    // sends response

    std::cout << "response: ";
    for (char c : response) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << ", ";
    }

    send(clientSocket, response.data(), response.size(), 0);


    return;
}

void parsePacket(ByteStream bs, int clientSocket)
{

    if (bs.readByte() != 0x30){
        return; // not a ldap packet
        std::cout << "0x30 missing" << std::endl;
    }
    //bs.setLenght(static_cast<int>(static_cast<unsigned char>(bs.readByte()))); // set lenght of packet

    bs.readByte(); // skip lenght

    if (bs.readByte() != 0x02) {
        return; // not a ldap packet
        std::cout << "0x02 missing" << std::endl;
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
            std::cout << "message id error" << std::endl;
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

            // TODO check if version is supported
            if (bs.readByte() != 0x03) return; // not suported version?

            if (bs.readByte() != 0x04) return; // error in bind request

            bs.readByte(); // skip byte

            // TODO chek rest of bind request
            //if (bs.readByte() != 0x80) return; // not a simple bind request

            //if (bs.readByte() != 0x00) return; // not a simple bind request

            // craft bind response backwards and sends it
            response = {0x00, 0x04, 0x00, 0x04, 0x00, 0x01, 0x0a};
            response.push_back(static_cast<char>(response.size()));
            response.push_back(0x61);

            sendPacketLDAP(response, clientSocket, bs.getMessageID());

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

            if (bs.readByte() != 0x01) return; // error in search request

            bs.readByte(); // skip size limit

            if (bs.readByte() != 0x02) return; // error in search request

            if (bs.readByte() != 0x01) return; // error in search request

            bs.readByte(); // skip time limit

            if (bs.readByte() != 0x01) return; // error in search request

            if (bs.readByte() != 0x01) return; // error in search request

            bs.readByte(); // skip types only

            std::cout << "now parsing filter" << std::endl;
            // parsing filter

            
            std::vector<unsigned char> filter;
            std::vector<attributeType_t> attributes;

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

            //char attributesLenght = bs.readByte();
            //if (attributesLenght == 0x00){
            //    attributes.push_back(UID);
            //    attributes.push_back(CN);
            //    attributes.push_back(MAIL);
            //}

            attributes.push_back(UID);
            attributes.push_back(CN);
            attributes.push_back(MAIL);

            searchTree filterTree = searchTree(filter, attributes, bs.getMessageID(), clientSocket);


            //TODO extract attributes from buffer


            // sends search result done
            response = {0x00, 0x04, 0x00, 0x04, 0x00, 0x01, 0x0a};
            response.push_back(static_cast<char>(response.size()));
            response.push_back(0x65);

            std::cout << "sending searchResDone" << std::endl;

            sendPacketLDAP(response, clientSocket, bs.getMessageID());

            break;
        }

        case 0x42: {
            // unbind request

            std::cout << "unbind request, BYE" << std::endl;

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

