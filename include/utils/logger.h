#ifndef LOGGER_H
#define LOGGER_H

//#define ENABLE_LOGGING
//#define LOG_LEVEL_INFO

// Log level macros
#if defined(LOG_LEVEL_TRACE)
    #define TRACE(x, ...) printf("[TRACE: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define DEBUG(x, ...) printf("[DEBUG: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define INFO(x, ...) printf("[INFO: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define WARN(x, ...) printf("[WARN: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define ERROR(x, ...) printf("[ERROR: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)

#elif defined(LOG_LEVEL_DEBUG)
    #define TRACE(x, ...)
    #define DEBUG(x, ...) printf("[DEBUG: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define INFO(x, ...) printf("[INFO: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define WARN(x, ...) printf("[WARN: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define ERROR(x, ...) printf("[ERROR: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)

#elif defined(LOG_LEVEL_INFO)
    #define TRACE(x, ...)
    #define DEBUG(x, ...)
    #define INFO(x, ...) printf("[INFO: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define WARN(x, ...) printf("[WARN: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define ERROR(x, ...) printf("[ERROR: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)

#elif defined(LOG_LEVEL_WARN)
    #define TRACE(x, ...)
    #define DEBUG(x, ...)
    #define INFO(x, ...)
    #define WARN(x, ...) printf("[WARN: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define ERROR(x, ...) printf("[ERROR: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)

#elif defined(LOG_LEVEL_ERROR)
    #define TRACE(x, ...)
    #define DEBUG(x, ...)
    #define INFO(x, ...)
    #define WARN(x, ...)
    #define ERROR(x, ...) printf("[ERROR: %s:%d] " x "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)

#else
    #define TRACE(x, ...)
    #define DEBUG(x, ...)
    #define INFO(x, ...)
    #define WARN(x, ...)
    #define ERROR(x, ...)
#endif

#endif // LOGGER_H
