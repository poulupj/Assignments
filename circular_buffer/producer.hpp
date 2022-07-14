#ifndef _PRODUCER_HPP_
#define _PRODUCER_HPP_

#include <memory>

#include "circular_buffer_interface.hpp"
#include "circular_buffer.hpp"

class Producer
{
public:
    Producer(int dataMax, std::shared_ptr<CircularBuffer> circularBuffer);

    /*
     * @brief Start producing and write to buffer
     */
    void produce();

    ~Producer() {}

private:
    // Current data to be written to buffer
    int data;

    // Maximum value of data
    int dataMax;

    // Pointer to buffer
    std::shared_ptr<CircularBuffer> circularBuffer;
};

#endif