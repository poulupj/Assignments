#ifndef _CIRCULAR_BUFFER_HPP_
#define _CIRCULAR_BUFFER_HPP_

#include <vector>
#include <mutex>
#include <atomic>

#include "circular_buffer_interface.hpp"

class CircularBuffer : public ICircularBuffer
{
public:
    CircularBuffer(const int n);

    bool isEmpty() const override;

    bool isFull() const override;

    bool write(int data) override;

    bool read(int& read) override;

    void writeProducerFinished() override;

    bool readProducerFinished() override;

    ~CircularBuffer() {}

private:
    // Buffer
    std::vector<int> buffer;

    // Front and Back pointers
    int front, back;

    // Size of buffer
    int size;

    // Mutex to lock access to buffer
    std::mutex bufferLock;

    // Status of production
    std::atomic<bool> producerFinished;
};

#endif