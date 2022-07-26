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

void Server::setUp()
{
    // Address of server
    struct sockaddr_in serverAddress = {};

    // Create the socket, resuse if already used.
    int option = 1;
    m_connectionFD = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(m_connectionFD, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (m_connectionFD < 0)
    {
        error("Error in opening socket\n");
    }

    // Initialize server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(m_ipAddress.c_str());
    serverAddress.sin_port = htons(m_portNumber);

    if ((bind(m_connectionFD, (struct sockaddr *)&serverAddress,
              sizeof(serverAddress))) < 0)
    {
        error("Error in binding socket\n");
    }
}

void Server::openConnection()
{
    socklen_t clientLength;
    // Start listening

    listen(m_connectionFD, 0);

    clientLength = sizeof(m_clientAddress);

    // Accept a connection
    m_transmissionFD = accept(m_connectionFD,
                              (struct sockaddr *)&m_clientAddress, &clientLength);

    if (m_transmissionFD < 0)
    {
        error("Error in accepting connection\n");
    }
}

void Server::closeConnection()
{
    // Close transmission socket once the client disconnects
    close(m_transmissionFD);
}

void Server::receiveCommand()
{

    char buffer[256];
    clearBuffer(buffer, 256);

    int status = read(m_transmissionFD, buffer, 255);

    if (status < 0)
    {
        error("Error in reading from the socket\n");
    }

    // buffer[strlen(buffer) - 1] = '\0';

    // printf("Received command: %s\n", buffer);

    // If exit is passed, terminate the connection
    if (strcmp(buffer, "exit") == 0)
    {
        closeConnection();
        return;
    }

    std::string output = execute(buffer);

    if (output.size() == 0)
    {
        output = "Success\n";
    }

    status = write(m_transmissionFD, output.c_str(), output.size());

    if (status < 0)
    {
        error("Error in writing to socket\n");
    }
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

        // split the commands to make it of the form char *const argv[] to pass to execv
        std::vector<std::string> commandSplit = splitCommand(recievedCommand);

        char **args = (char **)malloc((commandSplit.size() + 1) * sizeof(char *));

        int index = 0;
        for (auto arg : commandSplit)
        {
            args[index++] = strdup(arg.c_str());
        }
        args[index] = NULL;

        // path to the command to be executed
        char path[50] = "/bin/";
        strcat(path, args[0]);

        // Redirect the output to pipe to parent
        dup2(pipeCToP[1], 1);
        dup2(pipeCToP[1], 2);

        close(pipeCToP[1]);

        // Execute the command
        execv(path, args);

        free(args);

        _exit(EXIT_SUCCESS);
    }
    else
    {
        // If parent,

        // Close the pipes not required
        close(pipePToC[0]);
        close(pipeCToP[1]);

        write(pipePToC[1], command, strlen(command));

        close(pipePToC[1]);

        // Wait for child to finish
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

Server::~Server()
{
    closeConnection();
    close(m_connectionFD);
}
