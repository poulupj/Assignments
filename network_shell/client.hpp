#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include <string>

class Client
{
public:
    Client();

    /**
     * @brief A helper function to clear a character buffer
     */
    void clearBuffer(char *buffer, const int size) const;

    /**
     * @brief A helper function to report errors
     */
    void error(const char *message) const;

    /**
     * @brief To connect to a server
     */
    void connectToServer();

    /**
     * @brief To disconnect from the server
     */
    void disconnectFromServer();

    /**
     * @brief To send a command to the server
     */
    std::string executeCommand(const char buffer[256]);

    ~Client();

private:
    // Port number
    int m_portNumber = 9000;

    // IP address
    std::string m_ipAddress = "127.0.0.1";

    // Socket to transmit data
    int m_transmissionFD = -1;
};

#endif