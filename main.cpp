#include "mbed.h"
#include "ModelExecutor.h"

int main()
{
	ModelExecutor executor;

	executor.initRuntime();
	Result<torch::executor::Program> program = executor.loadModelBuffer();
	executor.runModel(program);

	// main() is expected to loop forever.
	// If main() actually returns the processor will halt

	return 0;
}