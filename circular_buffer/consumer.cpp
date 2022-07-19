#include <fstream>

#include "consumer.hpp"
#include "circular_buffer_interface.hpp"
#include "circular_buffer.hpp"

Consumer::Consumer(int id, std::shared_ptr<CircularBuffer> circularBuffer) : 
    id(id), data(0), circularBuffer(circularBuffer) {}

void Consumer::consume()
{
    std::ofstream file("file" + std::to_string(id) + ".txt");

    while (true)
    {
        // Continuously try to read until true is returned.
        while (!circularBuffer->read(data))
        {
            // If buffer is empty and producer has stopped, stop consuming.
            if (circularBuffer->IsReaderDone() == true)
            {
                file.close();
                return;
            }
        }

        file << data << "\n";
    }
}