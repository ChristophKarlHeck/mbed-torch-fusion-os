#include "mbed.h"

namespace mbed_lib {

    void print_memory_usage() {
        // Print heap statistics
        mbed_stats_heap_t heap_stats;
        mbed_stats_heap_get(&heap_stats);
        printf("Heap size: %llu, Used: %llu, Max used: %llu\n",
            static_cast<unsigned long long>(heap_stats.reserved_size),
            static_cast<unsigned long long>(heap_stats.current_size),
            static_cast<unsigned long long>(heap_stats.max_size));

        // Print stack statistics for all threads
        const size_t max_threads = MBED_CONF_RTOS_THREAD_NUM; // Configured maximum threads
        mbed_stats_stack_t stack_stats[max_threads];
        size_t thread_count = mbed_stats_stack_get_each(stack_stats, max_threads);

        for (size_t i = 0; i < thread_count; i++) {
            printf("Thread ID: %llu, Stack size: %llu, Max usage: %llu\n",
                static_cast<unsigned long long>(stack_stats[i].thread_id),
                static_cast<unsigned long long>(stack_stats[i].reserved_size),
                static_cast<unsigned long long>(stack_stats[i].max_size));
        }
    }
} // namespace mbed_lib
