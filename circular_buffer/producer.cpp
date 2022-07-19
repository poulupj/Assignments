#include <thread>

#include "producer.hpp"
#include "circular_buffer_interface.hpp"
#include "circular_buffer.hpp"

Producer::Producer(int dataMax, std::shared_ptr<CircularBuffer> circularBuffer) : 
    data(1), dataMax(dataMax), circularBuffer(circularBuffer) {}

void Producer::produce()
{
    while (data <= dataMax)
    {
        while (!circularBuffer->write(data));

        data++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    circularBuffer->IsWriterDone();
}