/* Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 * Copyright 2023-2024 Arm Limited and/or its affiliates.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <stdio.h>
#include <memory>
#include <vector>
#include <stdint.h>

#include <executorch/extension/data_loader/buffer_data_loader.h>
#include <executorch/extension/runner_util/inputs.h>
#include <executorch/runtime/executor/program.h>
#include <executorch/runtime/platform/log.h>
#include <executorch/runtime/platform/platform.h>
#include <executorch/runtime/platform/runtime.h>

/**
 * This header file is generated by the build process based on the .pte file
 * specified in the ET_PTE_FILE_PATH variable to the cmake build.
 * Control of the action of the .pte, it's use of operators and delegates, and
 * which are included in the bare metal build are also orchestrated by the
 * CMakeLists file. For example use see examples/arm/run.sh
 */
#include "softmax/model_pte.h"
#include "model_executor/ModelExecutor.h"

using namespace exec_aten;
using namespace std;
using torch::executor::Error;
using torch::executor::Result;

#define METHOD_ALLOCATOR_POOL_SIZE (512)

void et_pal_init(void) {}

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

// Assume SystemCoreClock is defined correctly for your platform.
extern uint32_t SystemCoreClock;

void delay_ms(uint32_t ms) {
    uint32_t ticks = (SystemCoreClock / 1000) * ms;
    for (uint32_t i = 0; i < ticks; ++i) {
        __asm volatile("nop"); // nop is an assembly instruction that does nothing but consume one clock cycle.
    }
}

ModelExecutor::ModelExecutor(size_t pool_size)
    : m_method_allocator_pool(nullptr), m_allocator_pool_size(pool_size) {
}

ModelExecutor::~ModelExecutor() {
}

ModelExecutor& ModelExecutor::getInstance(size_t pool_size) {
    static ModelExecutor instance(pool_size); // Singleton instance with customizable pool size
    return instance;
}

std::vector<float> ModelExecutor::run_model(std::vector<float> feature_vector){

		torch::executor::runtime_init();

		ET_LOG(Info, "Model in %p %c", model_pte, model_pte[0]);

		m_method_allocator_pool = (uint8_t*)malloc(m_allocator_pool_size);

		auto loader =
			torch::executor::util::BufferDataLoader(model_pte, sizeof(model_pte));
		ET_LOG(Info, "Model PTE file loaded. Size: %lu bytes.", sizeof(model_pte));
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
				m_allocator_pool_size, m_method_allocator_pool)};

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
		ET_LOG(Info, "Input prepared.");

		/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
		// Set variable input
		torch::executor::EValue input_original = method->get_input(0);
    	Tensor tensor = input_original.payload.as_tensor;
    	float* data = input_original.payload.as_tensor.mutable_data_ptr<float>();

    	// Change input
    	for(int j = 0; j < tensor.numel(); ++j){
        	data[j] = feature_vector[j];
    	}

		// Set input 
		method->set_input(input_original,0);

		/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

		ET_LOG(Info, "Starting the model execution...");
		//delay_ms(100);
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
		std::vector<float> results; // Vector to hold the output values
		for (uint i = 0; i < outputs.size(); ++i) {
			Tensor t = outputs[i].toTensor();
			for (int j = 0; j < outputs[i].toTensor().numel(); ++j) {
			if (t.scalar_type() == ScalarType::Int) {
				printf(
					"Output[%d][%d]: %d\n",
					i,
					j,
					outputs[i].toTensor().const_data_ptr<int>()[j]);
				results.push_back(outputs[i].toTensor().const_data_ptr<int>()[j]);
			} else {
				printf(
					"Output[%d][%d]: %f\n",
					i,
					j,
					outputs[i].toTensor().const_data_ptr<float>()[j]);
				results.push_back(outputs[i].toTensor().const_data_ptr<float>()[j]);
			}
			}
		}
		ET_LOG(Info, "Program complete, exiting.");
		// Freeing the memory
		free(m_method_allocator_pool);
		m_method_allocator_pool = nullptr; // Optional but recommended to avoid dangling pointers
		//delay_ms(100); // Delays for 100 milliseconds
	

	return results;
}