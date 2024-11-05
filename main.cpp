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
	executor.printModelOutput(method);

	// main() is expected to loop forever.
	// If main() actually returns the processor will halt

	return 0;
}