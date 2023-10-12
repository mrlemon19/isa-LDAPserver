// main.h
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#ifndef __MAIN_H__
#define __MAIN_H__

#include <iostream>
#include <string>
#include <filesystem>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

namespace fs = std::filesystem;

bool isValidPort (std::string port);
bool validDB(std::string fileStr, std::string &DBFile);
void handleClient(int clientSocket);

#endif // __MAIN_H__
