#include "circular_buffer.hpp"
#include "circular_buffer_interface.hpp"

CircularBuffer::CircularBuffer(int n) : 
    buffer(std::vector<int>(n / sizeof(int))), front(-1), back(-1), size(n / sizeof(int))
{
    producerFinished.store(false);
}

bool CircularBuffer::isEmpty() const
{
    return (front == -1);
}

bool CircularBuffer::isFull() const
{
    return ((back + 1) % size == front);
}

bool CircularBuffer::write(int data)
{
    std::lock_guard<std::mutex> lock(bufferLock);

    if (isFull())
    {
        return false;
    }

    if (isEmpty())
    {
        front = back = 0;
    }
    else
    {
        back = (back + 1) % size;
    }

    buffer[back] = data;

    return true;
}

bool CircularBuffer::read(int &data)
{
    std::lock_guard<std::mutex> lock(bufferLock);

    if (isEmpty())
    {
        return false;
    }

    data = buffer[front];

    if (front == back)
    {
        front = back = -1;
    }
    else
    {
        front = (front + 1) % size;
    }

    return true;
}

void CircularBuffer::writeProducerFinished()
{
    producerFinished.store(true);
}

bool CircularBuffer::readProducerFinished()
{
    return producerFinished.load();
}