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

    void IsWriterDone() override;

    bool IsReaderDone() override;

    ~CircularBuffer() = default;

private:
    // Buffer
    std::vector<int> m_buffer;

    // Front and Back pointers
    int m_front = -1, m_back = -1;

    // Size of buffer
    int m_size;

    // Mutex to lock access to buffer
    std::mutex m_bufferLock;

    // Status of production
    std::atomic<bool> m_producerFinished = ATOMIC_VAR_INIT(false);
};

#endif