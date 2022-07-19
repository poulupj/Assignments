#ifndef _CIRCULAR_BUFFER_INTERFACE_HPP_
#define _CIRCULAR_BUFFER_INTERFACE_HPP_

class ICircularBuffer
{
public:
    /**
     * @brief A helper function to check if buffer is empty
     */
    virtual bool isEmpty() const = 0;

    /**
     * @brief A helper function to check if buffer is full
     */

    virtual bool isFull() const = 0;

    /**
     * @brief To write into the buffer
     */
    virtual bool write(int data) = 0;

    /**
     * @brief To read from the buffer
     */
    virtual bool read(int& data) = 0;

    /**
     * @brief A helper function to to let buffer know production is complete
     */
    virtual void IsWriterDone() = 0;

    /**
     * @brief A helper function to check if production is complete
     */
    virtual bool IsReaderDone() = 0;

    /* @brief destructor */
    virtual ~ICircularBuffer() = default;
};

#endif