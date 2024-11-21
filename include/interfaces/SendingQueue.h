#ifndef SENDING_QUEUE_H
#define SENDING_QUEUE_H

#include "mbed.h"
#include <vector>
#include <array>

class SendingQueue {
public:

    // Static method to access the single instance
    static SendingQueue& getInstance();

    // The structure used for inter-thread communication
    typedef struct {
        std::vector<std::array<uint8_t, 3>> inputs;  // Vector of downsampled analog values
        bool channel;                                // Channel information
    } mail_t;

    // Mail object for inter-thread communication
    Mail<mail_t, 4> mail_box;  // Queue with size 4, can be adjusted as necessary

private:
    // Private constructor to prevent direct instantiation
    SendingQueue(void);

    // Private destructor (optional)
    ~SendingQueue();

    // Deleted copy constructor and assignment operator to prevent copies
    SendingQueue(const SendingQueue&) = delete;
    SendingQueue& operator=(const SendingQueue&) = delete;
};

#endif // SENDING_QUEUE_H
