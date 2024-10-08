#include "mbed.h"
#include <stdio.h>
#include <memory>
#include <vector>

#include <executorch/extension/data_loader/buffer_data_loader.h>
#include <executorch/extension/runner_util/inputs.h>
#include <executorch/runtime/executor/program.h>
#include <executorch/runtime/platform/log.h>
#include <executorch/runtime/platform/platform.h>
#include <executorch/runtime/platform/runtime.h>

#include "model_pte.h"

using namespace exec_aten;
using namespace std;
using torch::executor::Error;
using torch::executor::Result;

__attribute__((section(".sram.data"), aligned(16)))
uint8_t method_allocator_pool[4 * 1024U];

//void et_pal_init(void) {}

__ET_NORETURN void et_pal_abort(void) {
  __builtin_trap();
}

/**
 * Emit a log message via platform output (serial port, console, etc).
 */
void et_pal_emit_log_message(
    __ET_UNUSED et_timestamp_t timestamp,
    et_pal_log_level_t level,
    const char* filename,
    __ET_UNUSED const char* function,
    size_t line,
    const char* message,
    __ET_UNUSED size_t length) {
  fprintf(stderr, "%c executorch:%s:%zu] %s\n", level, filename, line, message);
}

int main()
{
	torch::executor::runtime_init();

	auto loader =
		torch::executor::util::BufferDataLoader(model_pte, sizeof(model_pte));
	//ET_LOG(Info, "Model PTE file loaded. Size: %lu bytes.", sizeof(model_pte));
	Result<torch::executor::Program> program =
		torch::executor::Program::load(&loader);
	if (!program.ok()) {
		ET_LOG(
			Info,
			"Program loading failed @ 0x%p: 0x%" PRIx32,
			model_pte,
			program.error());
	}

	ET_LOG(Info, "Model buffer loaded, has %lu methods", program->num_methods());

	const char* method_name = nullptr;
	{
		const auto method_name_result = program->get_method_name(0);
		ET_CHECK_MSG(method_name_result.ok(), "Program has no methods");
		method_name = *method_name_result;
	}
	ET_LOG(Info, "Running method %s", method_name);

	Result<torch::executor::MethodMeta> method_meta =
		program->method_meta(method_name);
	if (!method_meta.ok()) {
		ET_LOG(
			Info,
			"Failed to get method_meta for %s: 0x%x",
			method_name,
			(unsigned int)method_meta.error());
	}

	torch::executor::MemoryAllocator method_allocator{
		torch::executor::MemoryAllocator(
			sizeof(method_allocator_pool), method_allocator_pool)};

	std::vector<std::unique_ptr<uint8_t[]>> planned_buffers; // Owns the memory
	std::vector<torch::executor::Span<uint8_t>>
		planned_spans; // Passed to the allocator
	size_t num_memory_planned_buffers = method_meta->num_memory_planned_buffers();

	for (size_t id = 0; id < num_memory_planned_buffers; ++id) {
		size_t buffer_size =
			static_cast<size_t>(method_meta->memory_planned_buffer_size(id).get());
		ET_LOG(Info, "Setting up planned buffer %zu, size %zu.", id, buffer_size);

		planned_buffers.push_back(std::make_unique<uint8_t[]>(buffer_size));
		planned_spans.push_back({planned_buffers.back().get(), buffer_size});
	}

	torch::executor::HierarchicalAllocator planned_memory(
		{planned_spans.data(), planned_spans.size()});

	torch::executor::MemoryManager memory_manager(
		&method_allocator, &planned_memory);

	Result<torch::executor::Method> method =
		program->load_method(method_name, &memory_manager);
	if (!method.ok()) {
		ET_LOG(
			Info,
			"Loading of method %s failed with status 0x%" PRIx32,
			method_name,
			method.error());
	}
	ET_LOG(Info, "Method loaded.");

	ET_LOG(Info, "Preparing inputs...");
	auto inputs = torch::executor::util::prepare_input_tensors(*method);
	if (!inputs.ok()) {
		ET_LOG(
			Info,
			"Preparing inputs tensors for method %s failed with status 0x%" PRIx32,
			method_name,
			inputs.error());
	}

	// Meta API
	const char* meta_name = method_meta->name();
	size_t num_inputs = method_meta->num_inputs();
	auto input_tag = method_meta->input_tag(0);
	auto input_tensor_meta = method_meta->input_tensor_meta(0);
	size_t num_outputs = method_meta->num_outputs();
	auto output_tag = method_meta->output_tag(0);
	auto output_tensor_meta = method_meta->output_tensor_meta(0);
	// Meta API END

	size_t input_size = method->inputs_size();
	const torch::executor::EValue input_original = method->get_input(0);
	Tensor tensor = input_original.payload.as_tensor;
	float* data = input_original.payload.as_tensor.mutable_data_ptr<float>();

	// Change input
	int j;
	for(j = 0; j < tensor.numel(); ++j){
		data[j] = 3.0; 
	}

	// Set input 
	method->set_input(input_original,0);
	const torch::executor::EValue input_new = method->get_input(0);
	
	// Check if input has changed
	for (int i = 0; i < input_size; ++i) {
		Tensor te = input_new.payload.as_tensor;
		for (int j = 0; j < te.numel(); ++j) { // numel returns the number of elements in the tensor
			if (te.scalar_type() == ScalarType::Int) {
				printf(
					"Input[%d][%d]: %d\n",
					i,
					j,
					te.const_data_ptr<int>()[j]);
			} else {
				printf(
					"Input[%d][%d]: %f\n",
					i,
					j,
					te.const_data_ptr<float>()[j]);
			}
		}
	}

	ET_LOG(Info, "Starting the model execution...");
	Error status = method->execute();
	if (status != Error::Ok) {
		ET_LOG(
			Info,
			"Execution of method %s failed with status 0x%" PRIx32,
			method_name,
			status);
	} else {
		ET_LOG(Info, "Model executed successfully.");
	}

	std::vector<torch::executor::EValue> outputs(method->outputs_size());
	ET_LOG(Info, "%zu outputs: ", outputs.size());
	status = method->get_outputs(outputs.data(), outputs.size());
	ET_CHECK(status == Error::Ok);
	for (int i = 0; i < outputs.size(); ++i) {
		Tensor t = outputs[i].toTensor();
		for (int j = 0; j < outputs[i].toTensor().numel(); ++j) {
		if (t.scalar_type() == ScalarType::Int) {
			printf(
				"Output[%d][%d]: %d\n",
				i,
				j,
				outputs[i].toTensor().const_data_ptr<int>()[j]);
		} else {
			printf(
				"Output[%d][%d]: %f\n",
				i,
				j,
				outputs[i].toTensor().const_data_ptr<float>()[j]);
		}
		}
	}

	// while(true) 
	// {
	// 	printf("well done!\n");
	// 	ThisThread::sleep_for(1s);
	// }

	// main() is expected to loop forever.
	// If main() actually returns the processor will halt
	return 0;
}