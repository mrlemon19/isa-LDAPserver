// parsePacket.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "parsePacket.h"

void parsePacket(ByteStream bs, int clientSocket, std::string DBfileName) {

  // packet sender init
  packetSender *ps = new packetSender(clientSocket);

  if (bs.readByte() != 0x30) {
    return; // not a ldap packet
  }

  bs.readByte(); // skip lenght

  if (bs.readByte() != 0x02) {
    return; // not a ldap packet
  }

  switch (bs.readByte()) {
  // read message id
  case 0x01:
    bs.setMessageID(
        static_cast<int>(static_cast<unsigned char>(bs.readByte())));
    break;

  case 0x02:
    bs.setMessageID(
        static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 8 |
        static_cast<int>(static_cast<unsigned char>(bs.readByte())));
    break;

  case 0x03:
    bs.setMessageID(
        static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 16 |
        static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 8 |
        static_cast<int>(static_cast<unsigned char>(bs.readByte())));
    break;

  case 0x04:
    bs.setMessageID(
        static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 24 |
        static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 16 |
        static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 8 |
        static_cast<int>(static_cast<unsigned char>(bs.readByte())));
    break;

  default:
    // ignore not valid ldap message
    return;
  }

  std::vector<unsigned char> response;
  unsigned char versionLDAP;

  // protocol op
  switch (bs.readByte()) {
  case 0x60:
    // bind request

    bs.readByte(); // skip lenght

    if (bs.readByte() != 0x02)
      return; // error in bind request

    if (bs.readByte() != 0x01)
      return; // error in bind request

    // read version
    versionLDAP = bs.readByte();
    if (versionLDAP != 0x03 && versionLDAP != 0x02)
      return; // not suported version?

    if (bs.readByte() != 0x04)
      return; // error in bind request

    bs.readByte(); // skip byte

    if (bs.readByte() != 0x80)
      return; // not a simple bind request

    if (bs.readByte() != 0x00)
      return; // not a simple bind request

    // craft bind response backwards and sends it via packet sender
    response = {0x00, 0x04, 0x00, 0x04, 0x00, 0x01, 0x0a};
    response.push_back(static_cast<unsigned char>(response.size()));
    response.push_back(0x61);

    ps->sendPacketLDAP(response, bs.getMessageID());

    break;

  case 0x63: {
    // search request

    bs.readByte(); // skip lenght

    if (bs.readByte() != 0x04)
      return; // error in search request

    bs.readByte(); // skip for now

    if (bs.readByte() != 0x0a)
      return; // error in search request

    if (bs.readByte() != 0x01)
      return; // error in search request

    bs.readByte(); // skip ldap scope

    if (bs.readByte() != 0x0a)
      return; // error in search request

    if (bs.readByte() != 0x01)
      return; // error in search request

    bs.readByte(); // skip ldap deref aliases

    if (bs.readByte() != 0x02)
      return; // error in search request

    // sizelimit handle
    int sizeLimit;

    switch (bs.readByte()) {
    case 0x01: {
      sizeLimit = static_cast<int>(static_cast<unsigned char>(bs.readByte()));
      break;
    }
    case 0x02: {
      sizeLimit = static_cast<int>(static_cast<unsigned char>(bs.readByte()))
                      << 8 |
                  static_cast<int>(static_cast<unsigned char>(bs.readByte()));
      break;
    }
    case 0x03: {
      sizeLimit =
          static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 16 |
          static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 8 |
          static_cast<int>(static_cast<unsigned char>(bs.readByte()));
      break;
    }
    case 0x04: {
      sizeLimit =
          static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 24 |
          static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 16 |
          static_cast<int>(static_cast<unsigned char>(bs.readByte())) << 8 |
          static_cast<int>(static_cast<unsigned char>(bs.readByte()));
      break;
    }
    default: {
      return; // error in search request
    }
    }

    if (sizeLimit == 0)
      sizeLimit = 100;

    if (bs.readByte() != 0x02)
      return; // error in search request

    if (bs.readByte() != 0x01)
      return; // error in search request

    bs.readByte(); // skip time limit

    if (bs.readByte() != 0x01)
      return; // error in search request

    if (bs.readByte() != 0x01)
      return; // error in search request

    bs.readByte(); // skip types only

    // parsing filter

    std::vector<unsigned char> filter;
    std::vector<unsigned char> attributes;

    // TODO catch when there is no filter or attributes

    filter.push_back(bs.readByte());
    unsigned char lenght = bs.readByte(); // lenght of filter
    if (lenght == 0x00)
      return; // no filter
    filter.push_back(lenght);
    // moving filter to vector
    for (int i = 0; i < static_cast<int>(static_cast<unsigned char>(lenght));
         i++) {
      filter.push_back(bs.readByte());
    }

    // parsing attributes

    if (bs.readByte() != 0x30)
      return; // error in search request

    int attributesLenght = bs.readByte(); // lenght of attributes

    for (int i = 0; i < attributesLenght; i++) {
      attributes.push_back(bs.readByte());
    }

    searchTree filterTree = searchTree(filter, attributes, bs.getMessageID(),
                                       clientSocket, DBfileName, ps, sizeLimit);
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

    close(clientSocket); // unbind request, end connection
    return;

    break;
  }

  default: {
    // send error message not valid ldap message
    std::vector<unsigned char> errorMessage = {
        0x64, 0x65, 0x74, 0x72, 0x6F, 0x70, 0x70, 0x75, 0x73, 0x20,
        0x74, 0x6F, 0x6E, 0x20, 0x72, 0x65, 0x74, 0x6C, 0x69, 0x46};

    // push lenght
    errorMessage.push_back(static_cast<unsigned char>(errorMessage.size()));
    errorMessage.push_back(0x04);

    // push result code
    errorMessage.push_back(0x35);
    errorMessage.push_back(0x01);
    errorMessage.push_back(0x0a);

    errorMessage.push_back(0x01);
    errorMessage.push_back(0x0a);

    // push lenght and protocol op
    errorMessage.push_back(static_cast<unsigned char>(errorMessage.size()));
    errorMessage.push_back(0x65);

    // send the message
    ps->sendPacketLDAP(errorMessage, bs.getMessageID());

    close(clientSocket);
    return;
  }
  }

  return;
}
