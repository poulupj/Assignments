#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>
#include <fstream>

#include "circular_buffer.hpp"
#include "producer.hpp"
#include "consumer.hpp"

// Function to execute the producer as a thread
void producerHandler(int n, std::shared_ptr<CircularBuffer> circularBuffer)
{
    Producer producer(n, circularBuffer);
    producer.produce();
}

// Function to execute consumer as a thread
void consumerHandler(int id, std::shared_ptr<CircularBuffer> circularBuffer)
{
    Consumer consumer(id, circularBuffer);
    consumer.consume();
}

// Verify the output *.txt files.
bool checkOutputFiles(int n, int c)
{
    std::vector<bool> present(n + 1, false);
    int count = 0;
    int data = 0;

    for(int i = 0; i < c; i++)
    {
        std::ifstream file("file" + std::to_string(i + 1) + ".txt");
        
        while(file >> data)
        {
            if(data > n || data < 1)
            {
                return false;
            }
            else if(present[data] == true)
            {
                return false;
            }
            else
            {
                present[data] = true;
                count++;
            }
        }

        file.close();
    }

    return (count == n);
}

// tests with gtest framework
TEST(BufferTest, BufferEmptyAtEnd)
{
    int m = 1000;
    int n = 10000;
    int c = 3;

    auto circularBuffer = std::make_shared<CircularBuffer>(m);

    std::thread producer(producerHandler, n, circularBuffer);
    std::thread consumers[c];

    for(int i = 0; i < c; i++)
    {
        consumers[i] = std::thread(consumerHandler, i + 1, circularBuffer);
    }

    producer.join();

    for(int i = 0; i < c; i++)
    {
        consumers[i].join();
    }

    ASSERT_TRUE(circularBuffer->isEmpty());
}

TEST(OutputFile, CorrectNumbersPresent)
{
    int m = 1000;
    int n = 10000;
    int c = 3;

    auto circularBuffer = std::make_shared<CircularBuffer>(m);

    std::thread producer(producerHandler, n, circularBuffer);
    std::thread consumers[c];

    for(int i = 0; i < c; i++)
    {
        consumers[i] = std::thread(consumerHandler, i + 1, circularBuffer);
    }

    producer.join();

    for(int i = 0; i < c; i++)
    {
        consumers[i].join();
    }

    ASSERT_TRUE(checkOutputFiles(n, c));
}