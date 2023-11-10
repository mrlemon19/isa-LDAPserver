// main.cpp
// Lightweight Directory Access Protocol (LDAP) server implementation
// vut fit isa project 2023/2024
// @author: xlukas18

#include "main.h"
int MAX_CLIENTS = 10;
std::string DBFile;

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

void handleClient(int clientSocket) {
    unsigned char buffer[1024];

    while (true) {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            // Client disconnected or an error occurred
            break;
        }

        // Process received data

        unsigned char* pbuffer = buffer;
        ByteStream bs(pbuffer);

        parsePacket(bs, clientSocket, DBFile);

    }

    // Close the client socket
    close(clientSocket);
}

int main(int argc, char *argv[]) {

    int portNum = 389;

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

    // Create socket
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Set up server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNum);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed");
        close(serverSocket);
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        perror("Listening failed");
        close(serverSocket);
        return 1;
    }

    while (true) {
        // Accept a new client connection
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Accepting client connection failed");
            continue;
        }

        // Create a new thread to handle the client and immediately detach it
        std::thread(handleClient, clientSocket).detach();
    }

    // Close the server socket (this line will not be reached)
    close(serverSocket);

    return 0;

}
