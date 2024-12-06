#include "mbed.h"
#include "platform/mbed_thread.h"

namespace mbed_lib {

    #define MAX_THREAD_INFO 10

    void print_memory_usage() {
        // Print heap statistics
        mbed_stats_heap_t heap_info;
        mbed_stats_stack_t stack_info[ MAX_THREAD_INFO ];

        printf("\nMemoryStats:");
        mbed_stats_heap_get(&heap_info);
        printf("\n\tBytes allocated currently: %ld", heap_info.current_size);
        printf("\n\tMax bytes allocated at a given time: %ld", heap_info.max_size);
        printf("\n\tCumulative sum of bytes ever allocated: %ld", heap_info.total_size);
        printf("\n\tCurrent number of bytes allocated for the heap: %ld", heap_info.reserved_size);
        printf("\n\tCurrent number of allocations: %ld", heap_info.alloc_cnt);
        printf("\n\tNumber of failed allocations: %ld", heap_info.alloc_fail_cnt);

        mbed_stats_stack_get(&stack_info[0]);
        printf("\nCumulative Stack Info:");
        printf("\n\tMaximum number of bytes used on the stack: %ld", stack_info[0].max_size);
        printf("\n\tCurrent number of bytes allocated for the stack: %ld", stack_info[0].reserved_size);
        printf("\n\tNumber of stacks stats accumulated in the structure: %ld\n", stack_info[0].stack_cnt);

        mbed_stats_stack_get_each(stack_info, MAX_THREAD_INFO);
        printf("Thread Stack Info:\n");
        for (int i = 0; i < MAX_THREAD_INFO; i++) {
            if (stack_info[i].thread_id != 0) {
                printf("\n\tThread: %d", i);
                printf("\n\t\tThread Id: 0x%08lX", stack_info[i].thread_id);
                printf("\n\t\tMaximum number of bytes used on the stack: %ld", stack_info[i].max_size);
                printf("\n\t\tCurrent number of bytes allocated for the stack: %ld", stack_info[i].reserved_size);
                printf("\n\t\tNumber of stacks stats accumulated in the structure: %ld", stack_info[i].stack_cnt);
            }
        }
    }

    #define MAX_THREAD_STACK 384
    #define SAMPLE_TIME_MS   2000
    #define LOOP_TIME_MS     3000

    uint64_t prev_idle_time = 0;
    int32_t wait_time_ms = 5000;

    void print_cpu_stats()
    {
        mbed_stats_cpu_t stats;
        mbed_stats_cpu_get(&stats);

        // Calculate the percentage of CPU usage
        uint64_t diff_usec = (stats.idle_time - prev_idle_time);
        uint8_t idle = (diff_usec * 100) / (SAMPLE_TIME_MS*1000);
        uint8_t usage = 100 - ((diff_usec * 100) / (SAMPLE_TIME_MS*1000));
        prev_idle_time = stats.idle_time;
        printf("\nCPU Info:\n");
        printf("Time(us): Up: %lld", stats.uptime);
        printf("   Idle: %lld", stats.idle_time);
        printf("   Sleep: %lld", stats.sleep_time);
        printf("   DeepSleep: %lld\n", stats.deep_sleep_time);
        printf("Idle: %d%% Usage: %d%%\n\n", idle, usage);
    }
} // namespace mbed_lib
