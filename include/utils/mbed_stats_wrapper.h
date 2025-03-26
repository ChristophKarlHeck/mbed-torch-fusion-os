#ifndef MBED_STATS_WRAPPER_H
#define MBED_STATS_WRAPPER_H

namespace mbed_lib {
    void print_memory_usage();
    void print_cpu_stats();
    void print_memory_info(const char *label);
}

#endif // MBED_STATS_WRAPPER_H
