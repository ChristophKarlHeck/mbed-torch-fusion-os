/*
Change the values of the following variables in the file: mbed-os/connectivity/FEATUR_BLE/source/cordio/mbed_lib.json
- "desired-att-mtu": 250
- "rx-acl-buffer-size": 255
- PB_6 and PB_7 are reserevd for CONSOLE_TX and CNSOLE_RX
*/

// Standard Library Headers
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

// Third-Party Library Headers
#include "mbed.h"

// Project-Specific Headers
#include "model_executor/ModelExecutor.h"


void print_heap_stats() {

	printf("\nMemoryStats:");
    mbed_stats_heap_t heap_stats;
    mbed_stats_heap_get(&heap_stats);
    printf("\n\tBytes allocated currently: %ld", heap_stats.current_size);
    printf("\n\tMax bytes allocated at a given time: %ld", heap_stats.max_size);
    printf("\n\tCumulative sum of bytes ever allocated: %ld", heap_stats.total_size);
    printf("\n\tCurrent number of bytes allocated for the heap: %ld", heap_stats.reserved_size);
    printf("\n\tCurrent number of allocations: %ld", heap_stats.alloc_cnt);
    printf("\n\tNumber of failed allocations: %ld", heap_stats.alloc_fail_cnt);

	printf("\nCumulative Stack Info:");
	mbed_stats_stack_t stack_stats;
	mbed_stats_stack_get(&stack_stats);
    printf("\n\tMaximum number of bytes used on the stack: %ld", stack_stats.max_size);
    printf("\n\tCurrent number of bytes allocated for the stack: %ld", stack_stats.reserved_size);
    printf("\n\tNumber of stacks stats accumulated in the structure: %ld", stack_stats.stack_cnt);
}

int main()
{	
	printf("start\n");

	// Instantiate and initialize the model executor
    ModelExecutor& model_executor = ModelExecutor::getInstance();
	model_executor.initRuntime();
    Result<torch::executor::Program> program = model_executor.loadModelBuffer();
    const char* method_name = model_executor.getMethodName(program);
    Result<torch::executor::MethodMeta> method_meta = model_executor.getMethodMeta(program, method_name);
    torch::executor::MemoryAllocator method_allocator = model_executor.getMemoryAllocator();
    std::vector<torch::executor::Span<uint8_t>> planned_spans = model_executor.setUpPlannedBuffer(program, method_meta);
    Result<torch::executor::Method> method = model_executor.loadMethod(program, method_allocator, planned_spans, method_name);
    //model_executor.prepareInputs(method, method_name);

	printf("Number of model input values: %u\n",model_executor.getNumberOfInputValues(method));

	
	std::vector<float> inputs = {3.4, 2.3, 3.1, 4.5};
	model_executor.setModelInput(method, inputs);
	print_heap_stats();
	model_executor.executeModel(method, method_name, 100);
	model_executor.printModelOutput(method);


	// main() is expected to loop forever.
	// If main() actually returns the processor will halt

	return 0;
}