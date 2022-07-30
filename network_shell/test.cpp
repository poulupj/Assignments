#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <thread>
#include <unistd.h>
#include <future>
#include <memory>

#include "server.hpp"
#include "client.hpp"

// Function to create a dummy file
void createDummyFile(const std::string &pathToFile, const int numberOfLines)
{
    std::ofstream file(pathToFile);

    int count = 0;
    long data = 100;

    while (count < numberOfLines)
    {
        file << data << "\n";
        data += 10;
        ++count;
    }
}

// Function to create the test directory
void generateDirectory(const std::filesystem::path &inputPath)
{
    std::filesystem::path pwd = std::filesystem::current_path() / inputPath;

    std::filesystem::create_directory(pwd);

    createDummyFile(pwd.string() + "/f1.txt", 100);
    createDummyFile(pwd.string() + "/f2.txt", 500);
    createDummyFile(pwd.string() + "/f3.txt", 40);
}

bool checkOutput(std::string actualOutput)
{
    if (actualOutput == "f1.txt\nf2.txt\nf3.txt\n")
        return true;
    else
        return false;
}

// A handler to run the server
void runServer()
{
    auto server = std::make_unique<Server>();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    server->openConnection();
    server->receiveCommand();
}

// A handler to run the client
void runClient(const char *command, std::promise<std::string> &&output)
{
    auto client = std::make_unique<Client>();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    output.set_value(client->executeCommand(command));

    client->disconnectFromServer();
}

// tests with gtest framework

TEST(CommandTest, ls)
{
    std::filesystem::path p = "sample_dir";
    generateDirectory(p);

    std::thread serverThread(runServer);
    std::this_thread::sleep_for(std::chrono::seconds(4));

    std::promise<std::string> output;
    auto _output = output.get_future();
    std::thread clientThread(runClient, "ls ./sample_dir", std::move(output));

    clientThread.join();

    std::string actualOutput = _output.get();

    serverThread.join();

    ASSERT_TRUE(checkOutput(actualOutput));

    std::filesystem::remove_all(std::filesystem::current_path() / p);
}

TEST(CommandTest, mkdir)
{
    std::thread serverThread(runServer);
    std::this_thread::sleep_for(std::chrono::seconds(4));

    std::promise<std::string> output;
    auto _output = output.get_future();
    std::thread clientThread(runClient, "mkdir sample_dir", std::move(output));

    clientThread.join();

    std::string actualOutput = _output.get();

    serverThread.join();

    ASSERT_TRUE(std::filesystem::exists(std::filesystem::current_path() / std::string("sample_dir/")));

    std::filesystem::remove_all(std::filesystem::current_path() / std::string("sample_dir"));
}