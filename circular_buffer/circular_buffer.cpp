#include "circular_buffer.hpp"
#include "circular_buffer_interface.hpp"

CircularBuffer::CircularBuffer(int n) : 
    m_buffer(std::vector<int>(n / sizeof(int))), m_size(n / sizeof(int)) {}

bool CircularBuffer::isEmpty() const
{
    return (m_front == -1);
}

bool CircularBuffer::isFull() const
{
    return ((m_back + 1) % m_size == m_front);
}

bool CircularBuffer::write(int data)
{
    std::lock_guard<std::mutex> lock(m_bufferLock);

    if (isFull())
    {
        return false;
    }

    if (isEmpty())
    {
        m_front = m_back = 0;
    }
    else
    {
        m_back = (m_back + 1) % m_size;
    }

    m_buffer[m_back] = data;

    return true;
}

bool CircularBuffer::read(int &data)
{
    std::lock_guard<std::mutex> lock(m_bufferLock);

    if (isEmpty())
    {
        return false;
    }

    data = m_buffer[m_front];

    if (m_front == m_back)
    {
        m_front = m_back = -1;
    }
    else
    {
        m_front = (m_front + 1) % m_size;
    }

    return true;
}

void CircularBuffer::IsWriterDone()
{
    m_producerFinished.store(true);
}

bool CircularBuffer::IsReaderDone()
{
    return m_producerFinished.load();
}