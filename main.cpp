// implementation of Lightweight Directory Access Protocol (LDAP) server
// vut fit isa project 2023/2024
// @author: xlukas18

#include "isa-ldapserver.h"

bool isValidPort (std::string port)
{
    // checks if given port number is a digit and is in range 1-65535
    for (char c : port)
    {
        if (!std::isdigit(c))
        {
            std::cerr << "Invalid port number given" << std::endl;
            return false;
        }
    }

    return (std::stoi(port) > 0 && std::stoi(port) < 65535);
}

bool validDB(std::string fileStr, std::string &DBFile)
{
    // checks if given file exists
    if (!fs::exists(fileStr))
        {
            std::cerr << "File " << fileStr << "does not exist" << std::endl;
            return false;
        }
    
    DBFile = fileStr;
    return true;
    
}

int main(int argc, char *argv[]) {

    int portNum = 389;
    std::string DBFile;

    // checks commandline arguments
    if (argc == 3)
    {
        if (std::string(argv[1]) != "-f")
        {
            std::cerr << "Invalid arguments" << std::endl;
            return 1;
        }

        if (!validDB(argv[2], DBFile)) return 1;
    }

    else if (argc == 5)
    {
        if (std::string(argv[1]) != "-p" || std::string(argv[3]) != "-f")
        {
            std::cerr << "Invalid arguments" << std::endl;
            return 1;
        }

        if (!isValidPort(argv[2]))
        {
            std::cerr << "Invalid port number given" << std::endl;
            return 1;
        }

        portNum = std::stoi(argv[2]);

        if (!validDB(argv[4], DBFile)) return 1;
    }

    else
    {
        std::cerr << "Invalid arguments" << std::endl;
        return 1;
    }

    return 0;

}
