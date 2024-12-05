namespace mbed_lib {
    #include "mbed.h"

    class MbedWrapper {
    public:
        static void print_memory_usage() {
            mbed_stats_heap_t heap_stats;
            mbed_stats_heap_get(&heap_stats);
            printf("Heap size: %llu, Used: %llu, Max used: %llu\n",
                   static_cast<unsigned long long>(heap_stats.reserved_size),
                   static_cast<unsigned long long>(heap_stats.current_size),
                   static_cast<unsigned long long>(heap_stats.max_size));
        }
    };
}

int main() {
    mbed_lib::MbedWrapper::print_memory_usage();
    return 0;
}