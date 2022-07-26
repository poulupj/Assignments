#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "server.hpp"

Server::Server()
{
    setUp();
}

void Server::clearBuffer(char *buffer, const int size) const
{
    bzero(buffer, size);
}

void Server::error(const char *message) const
{
    perror(message);
    exit(0);
}

void Server::setUp() const
{
    // Sockets for establishing connection and transmission
    int connectionFD, transmissionFD;

    socklen_t clientLength;

    char buffer[256];

    // Address of server and client
    struct sockaddr_in serverAddress = {}, clientAddress = {};

    connectionFD = socket(AF_INET, SOCK_STREAM, 0);

    if (connectionFD < 0)
    {
        error("Error in opening socket\n");
    }

    // Initialize server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(m_ipAddress.c_str());
    serverAddress.sin_port = htons(m_portNumber);

    if ((bind(connectionFD, (struct sockaddr *)&serverAddress,
              sizeof(serverAddress))) < 0)
    {
        error("Error in binding socket\n");
    }

    // Start listening
    while (true)
    {
        listen(connectionFD, 0);

        clientLength = sizeof(clientAddress);

        // Accept a connection
        transmissionFD = accept(connectionFD,
                                (struct sockaddr *)&clientAddress, &clientLength);
        
        if (transmissionFD < 0)
        {
            error("Error in accepting connection\n");
        }

        while (true)
        {
            clearBuffer(buffer, 256);

            int status = read(transmissionFD, buffer, 255);

            if (status < 0)
            {
                error("Error in reading from the socket\n");
            }

            printf("MESSAGE: %s\n", buffer);

            // If exit is passed, terminate the connection
            if (strcmp(buffer, "exit\n") == 0)
            {
                break;
            }
        }

        // Close transmission socket once the client disconnects
        close(transmissionFD);
    }

    close(connectionFD);
}

int main()
{
    Server s;

    return 0;
}