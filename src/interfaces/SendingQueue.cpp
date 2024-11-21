#include "SendingQueue.h"

// Static method to access the single instance
SendingQueue& SendingQueue::getInstance() {
    static SendingQueue instance;  // Guaranteed to be destroyed, initialized on first use
    return instance;
}

// Private constructor to prevent direct instantiation
SendingQueue::SendingQueue(void) {
    // Initialization code, if necessary
}

// Private destructor (optional)
SendingQueue::~SendingQueue() {
    // Cleanup code, if necessary
}

