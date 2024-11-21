#include "ReadingQueue.h"

// Static method to access the single instance
ReadingQueue& ReadingQueue::getInstance() {
    static ReadingQueue instance;  // Guaranteed to be destroyed, initialized on first use
    return instance;
}

// Private constructor to prevent direct instantiation
ReadingQueue::ReadingQueue(void) {
    // Initialization code, if necessary
}

// Private destructor (optional)
ReadingQueue::~ReadingQueue() {
    // Cleanup code, if necessary
}
