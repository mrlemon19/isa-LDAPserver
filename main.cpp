// main.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
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
        std::cerr << "Invalid number of arguments" << std::endl;
        return 1;
    }


    // creates socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // socket binding
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(portNum);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        return 1;
    }

    // listening for connections
    if (listen(serverSocket, 5) < 0)
    {
        std::cerr << "Error listening on socket" << std::endl;
        close(serverSocket);
        return 1;
    }

    // accepting connections
    sockaddr_in clientAddress{};
    socklen_t clientAddressLen = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);

    if (clientSocket < 0)
    {
        std::cerr << "Error accepting connection" << std::endl;
        close(serverSocket);
        return 1;
    }

    // reading from socket
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        size_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
        {
            std::cerr << "Error reading from socket" << std::endl;
            break;
        }

        std::cout << "received:" << buffer << std::endl;

        // sending response
        std::string response = "OK";
        send(clientSocket, response.c_str(), response.size(), 0);


    }

    close(serverSocket);
    close(clientSocket);

    return 0;

}
