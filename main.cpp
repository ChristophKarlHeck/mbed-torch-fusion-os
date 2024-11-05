/*
Change the values of the following variables in the file: mbed-os/connectivity/FEATUR_BLE/source/cordio/mbed_lib.json
- "desired-att-mtu": 250
- "rx-acl-buffer-size": 255
*/

#include "mbed.h"
#include "ModelExecutor.h"

int main()
{
	ModelExecutor executor;

	executor.initRuntime();
	Result<torch::executor::Program> program = executor.loadModelBuffer();
	const char* method_name = executor.getMethodName(program);
	Result<torch::executor::MethodMeta> method_meta = executor.getMethodMeta(program, method_name);
	torch::executor::MemoryAllocator method_allocator = executor.getMemoryAllocator();
	std::vector<torch::executor::Span<uint8_t>> planned_spans = executor.setUpPlannedBuffer(program, method_meta);
	Result<torch::executor::Method> method = executor.loadMethod(program, method_allocator, planned_spans, method_name);
	executor.prepareInputs(method, method_name);

	// Get Inputs from Sensors. Size of array must be the same as tensor.numel/ number of input arguments in forward method
	std::vector<float> inputs = {1.0f, 2.0f, 3.0f, 4.0f};

	executor.setModelInput(method, inputs);
	executor.printModelInput(method);
	executor.executeModel(method, method_name);
	//executor.printModelOutput(method);

	std::vector<float> outputs = executor.getModelOutput(method);

  	// for (size_t i = 0; i < outputs.size(); ++i) {
    //     printf("Output[%zu]: %f\n", i, outputs[i]);
    // }

	// main() is expected to loop forever.
	// If main() actually returns the processor will halt

	return 0;
}