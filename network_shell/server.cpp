#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <iostream>

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
    exit(EXIT_FAILURE);
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

            buffer[strlen(buffer) - 1] = '\0';

            printf("MESSAGE: %s\n", buffer);

            // If exit is passed, terminate the connection
            if (strcmp(buffer, "exit") == 0)
            {
                break;
            }

            std::cout<<execute(buffer);
        }

        // Close transmission socket once the client disconnects
        close(transmissionFD);
    }

    close(connectionFD);
}

std::vector<std::string> Server::splitCommand(std::string command) const
{
    std::string space_delimiter = " ";
    std::vector<std::string> words;

    size_t pos = 0;
    while ((pos = command.find(space_delimiter)) != std::string::npos)
    {
        words.push_back(command.substr(0, pos));
        command.erase(0, pos + space_delimiter.length());
    }
    words.push_back(command);

    return words;
}

std::string Server::execute(char *command) const
{
    // A pipe to pass the command from the parent to the child
    int pipePToC[2];

    // A pipe to pass the result from the child to the parent
    int pipeCToP[2];

    pid_t processID;

    std::string receivedOutput = "";

    // Create the pipes
    if ((pipe(pipePToC) == -1))
    {
        error("Error in pipe creation\n");
    }

    if ((pipe(pipeCToP) == -1))
    {
        error("Error in pipe creation\n");
    }

    processID = fork();

    if (processID == -1)
    {
        error("Error in forking\n");
    }
    else if (processID == 0)
    {
        // If child process,

        // Close pipes that are not required
        close(pipePToC[1]);
        close(pipeCToP[0]);

        std::string recievedCommand = "";
        char temp;

        while (read(pipePToC[0], &temp, 1) > 0)
        {
            recievedCommand += temp;
        }

        close(pipePToC[0]);

        std::vector<std::string> commandSplit = splitCommand(recievedCommand);

        char **args = (char **)malloc((commandSplit.size() + 1) * sizeof(char *));

        int index = 0;
        for (auto arg : commandSplit)
        {
            args[index++] = strdup(arg.c_str());
        }
        args[index] = NULL;

        char path[50] = "/bin/";
        strcat(path, args[0]);

        dup2(pipeCToP[1], 1);
        dup2(pipeCToP[1], 2);

        close(pipeCToP[1]);

        execv(path, args);

        free(args);

        _exit(EXIT_SUCCESS);
    }
    else
    {
        close(pipePToC[0]);
        close(pipeCToP[1]);

        write(pipePToC[1], command, strlen(command));

        close(pipePToC[1]);

        int status;
        waitpid(processID, &status, 0);

        char temp;

        while (read(pipeCToP[0], &temp, 1) > 0)
        {
            receivedOutput += temp;
        }
        close(pipeCToP[0]);
    }

    return receivedOutput;
}

int main()
{
    Server s;

    return 0;
}