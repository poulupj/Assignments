#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <string>
#include <vector>

class Server{
public:
    Server();

    /**
     * @brief A helper function to clear a character buffer
     */
    void clearBuffer(char *buffer, const int size) const;

    /**
     * @brief A helper function to report errors
     */
    void error(const char *message) const;

    /**
     * @brief To set up a connection and start listening
     */
    void setUp() const;

    /**
     * @brief A helper function to split a command string 
     */
    std::vector<std::string> splitCommand(std::string command) const;

    /**
     * @brief A function to execute the command and return the result
     */

    std::string execute(char *command) const;

private:
    // Port number
    int m_portNumber = 9000;

    // IP address
    std::string m_ipAddress = "127.0.0.1";
};

#endif