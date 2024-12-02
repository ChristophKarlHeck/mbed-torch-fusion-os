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
    mbed_stats_heap_t heap_stats;
    mbed_stats_heap_get(&heap_stats);
    printf("Current heap size: %lu / %lu bytes\n", heap_stats.current_size, heap_stats.reserved_size);
    printf("Max heap size: %lu bytes\n", heap_stats.max_size);
    printf("Allocations: %lu\n", heap_stats.alloc_cnt);
    printf("Failures: %lu\n", heap_stats.alloc_fail_cnt);
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
    model_executor.prepareInputs(method, method_name);

	printf("Number of model input values: %u\n",model_executor.getNumberOfInputValues(method));

	int counter = 0;

    while (true) {

		// Execute Model with received inputs
		print_heap_stats();
		std::vector<float> inputs = {3.4, 2.3, 3.1, 4.5};
		model_executor.setModelInput(method, inputs);
		model_executor.printModelInput(method);
		model_executor.executeModel(method, method_name, 100);

		counter = counter + 1;
		printf("Counter: %d\n", counter);
		print_heap_stats();
	}
	

	// main() is expected to loop forever.
	// If main() actually returns the processor will halt

	return 0;
}