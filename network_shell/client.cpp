#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "client.hpp"

Client::Client()
{
    connectToServer();
}

void Client::clearBuffer(char *buffer, const int size) const
{
    bzero(buffer, size);
}

void Client::error(const char *message) const
{
    perror(message);
    exit(0);
}

void Client::connectToServer()
{
    // Address of server
    struct sockaddr_in serverAddress = {};

    char buffer[256];

    m_transmissionFD = socket(AF_INET, SOCK_STREAM, 0);

    if (m_transmissionFD < 0)
    {
        error("Error in opening socket\n");
    }

    // Initialize server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(m_ipAddress.c_str());
    serverAddress.sin_port = htons(m_portNumber);

    // Connect to the server
    if (connect(m_transmissionFD, (struct sockaddr *)&serverAddress,
                sizeof(serverAddress)) < 0)
    {
        error("Error in connecting\n");
    }
}

void Client::disconnectFromServer()
{
    close(m_transmissionFD);
}

std::string Client::executeCommand(const char buffer[256])
{
    // Send the command
    int status = write(m_transmissionFD, buffer, strlen(buffer));

    if (status < 0)
    {
        error("Error in writing to socket\n");
    }

    //clearBuffer(buffer, 256);

    if (strcmp(buffer, "exit\n") == 0)
    {
        return "";
    }

    char output[1000];
    clearBuffer(output, 1000);

    status = read(m_transmissionFD, output, 1000);

    if (status < 0)
    {
        error("Error in reading from the socket\n");
    }

    std::string s = output;
    return s;
}

Client::~Client()
{
    close(m_transmissionFD);
}

