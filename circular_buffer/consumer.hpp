#ifndef _CONSUMER_HPP_
#define _CONSUMER_HPP_

#include <memory>

#include "circular_buffer_interface.hpp"
#include "circular_buffer.hpp"

class Consumer 
{
public:
    Consumer(int id, std::shared_ptr<CircularBuffer> circularBuffer);

    /* 
     * @brief Consume from the buffer
     */
    void consume();

    ~Consumer() {}

private:
    // id of thread
    int id;

    // Data which is read
    int data;

    // Pointer to buffer
    std::shared_ptr<CircularBuffer> circularBuffer;
};

#endif