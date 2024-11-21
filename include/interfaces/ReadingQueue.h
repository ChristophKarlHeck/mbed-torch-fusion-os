#ifndef READING_QUEUE_H
#define READING_QUEUE_H

#include "mbed.h"
#include <vector>
#include <array>

class ReadingQueue {
public:

    // Static method to access the single instance
    static ReadingQueue& getInstance();

    // The structure used for inter-thread communication
    typedef struct {
        std::vector<std::array<uint8_t, 3>> inputs;  // Vector of downsampled analog values
        bool channel;                                // Channel information
    } mail_t;

    // Mail object for inter-thread communication
    Mail<mail_t, 4> mail_box;  // Queue with size 4, can be adjusted as necessary

private:
    // Private constructor to prevent direct instantiation
    ReadingQueue(void);

    // Private destructor (optional)
    ~ReadingQueue();

    // Deleted copy constructor and assignment operator to prevent copies
    ReadingQueue(const ReadingQueue&) = delete;
    ReadingQueue& operator=(const ReadingQueue&) = delete;
};

#endif // READING_QUEUE_H



