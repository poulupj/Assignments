#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

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

void Client::connectToServer() const
{
    // Socket for transmitting
    int transmissionFD;

    // Address of server
    struct sockaddr_in serverAddress = {};

    char buffer[256];

    transmissionFD = socket(AF_INET, SOCK_STREAM, 0);

    if (transmissionFD < 0)
    {
        error("Error in opening socket\n");
    }

    // Initialize server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(m_ipAddress.c_str());
    serverAddress.sin_port = htons(m_portNumber);

    // Connect to the server
    if (connect(transmissionFD, (struct sockaddr *)&serverAddress,
                sizeof(serverAddress)) < 0)
    {
        error("Error in connecting\n");
    }

    // Start sending to server
    while(true)
    {
        clearBuffer(buffer, 256);
        printf("Please enter message: ");

        fgets(buffer, 255, stdin);

        int status = write(transmissionFD, buffer, strlen(buffer));

        if(status < 0)
        {
            error("Error in writing to socket\n");
        }

        if(strcmp(buffer, "exit\n") == 0)
        {
            break;
        }
        
    }

    close(transmissionFD);
}

int main()
{
    Client c;

    return 0;
}