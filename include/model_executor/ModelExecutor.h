#ifndef MODELEXECUTOR_H
#define MODELEXECUTOR_H

#include <stdio.h>
#include <memory>
#include <vector>

#include <executorch/extension/data_loader/buffer_data_loader.h>
#include <executorch/extension/runner_util/inputs.h>
#include <executorch/runtime/executor/program.h>
#include <executorch/runtime/platform/log.h>
#include <executorch/runtime/platform/platform.h>
#include <executorch/runtime/platform/runtime.h>

//#include "mbed.h" // needed for thread_sleep_for(10);

using namespace exec_aten;
using namespace std;
using torch::executor::Error;
using torch::executor::Result;

/**
 * @class ModelExecutor
 * @brief Singleton class to manage PyTorch model execution on embedded systems.
 *
 * This class provides methods to initialize the runtime, load models, prepare inputs,
 * execute the model, and retrieve outputs. It leverages PyTorch's ExecutorCH runtime
 * for efficient inference.
 */
class ModelExecutor 
{
public:
    /**
     * @brief Provides access to the singleton instance of ModelExecutor.
     *
     * Ensures only one instance of ModelExecutor exists during runtime.
     * 
     * @return Reference to the singleton instance.
     */
    static ModelExecutor& getInstance() {
        static ModelExecutor instance; // Thread-safe in C++11 and later
        return instance;
    }

    /**
     * @brief Deletes the copy constructor to enforce singleton behavior.
     */
    ModelExecutor(const ModelExecutor&) = delete;

    /**
     * @brief Deletes the assignment operator to enforce singleton behavior.
     */
    ModelExecutor& operator=(const ModelExecutor&) = delete;

    /**
     * @brief Initializes the runtime environment for model execution.
     */
    void initRuntime(void);

    /**
     * @brief Loads a model from a buffer into memory.
     *
     * @return A Result object containing the loaded Program or an error.
     */
    Result<torch::executor::Program> loadModelBuffer(void);

    /**
     * @brief Retrieves the method name from a loaded program.
     *
     * @param program The program from which to extract the method name.
     * @return The name of the method.
     */
    const char* getMethodName(Result<torch::executor::Program>& program);

    /**
     * @brief Retrieves metadata for a specific method in the program.
     *
     * @param program The program containing the method.
     * @param method_name The name of the method to retrieve metadata for.
     * @return Metadata about the specified method.
     */
    Result<torch::executor::MethodMeta> getMethodMeta(
        Result<torch::executor::Program> &program,
        const char* method_name);

    /**
     * @brief Gets the memory allocator used for model execution.
     *
     * Provides access to the memory allocator for managing execution buffers.
     * 
     * @return A MemoryAllocator object.
     */
    torch::executor::MemoryAllocator getMemoryAllocator(void);

    /**
     * @brief Sets up planned memory buffers for a method in the program.
     *
     * Allocates and initializes memory buffers required for method execution.
     * 
     * @param program The program containing the method.
     * @param method_meta Metadata for the method.
     * @return A vector of spans representing the planned memory buffers.
     */
    std::vector<torch::executor::Span<uint8_t>> setUpPlannedBuffer(
        Result<torch::executor::Program> &program,
        Result<torch::executor::MethodMeta>& method_meta);

    /**
     * @brief Loads a method from the program into memory.
     *
     * @param program The program containing the method.
     * @param method_allocator The memory allocator for the method.
     * @param planned_spans The planned memory buffers.
     * @param method_name The name of the method to load.
     * @return A Result object containing the loaded method or an error.
     */
    Result<torch::executor::Method> loadMethod(
        Result<torch::executor::Program>& program,
        torch::executor::MemoryAllocator& method_allocator,
        std::vector<torch::executor::Span<uint8_t>>& planned_spans,
        const char* method_name);

    /**
     * @brief Retrieves the number of input values expected by the method.
     *
     * @param method The method to query.
     * @return The number of input values.
     */
    unsigned int getNumberOfInputValues(Result<torch::executor::Method>& method);

    /**
     * @brief Prepares input buffers for the specified method.
     *
     * @param method The method to prepare inputs for.
     * @param method_name The name of the method.
     */
    void prepareInputs(Result<torch::executor::Method>& method, const char* method_name);

    /**
     * @brief Sets the input values for the specified method.
     *
     * @param method The method to set inputs for.
     * @param inputs A vector of input values.
     */
    void setModelInput(Result<torch::executor::Method>& method, std::vector<float>& inputs);

    /**
     * @brief Prints the input values of the specified method for debugging purposes.
     *
     * @param method The method whose inputs to print.
     */
    void printModelInput(Result<torch::executor::Method>& method);

    /**
     * @brief Executes the loaded model with the specified parameters.
     *
     * @param method The method to execute.
     * @param method_name The name of the method to execute.
     * @param waiting_time The amount of time to wait before execution to avaoid special hard fault: https://github.com/mbed-ce/mbed-os/issues/395
     */
    void executeModel(Result<torch::executor::Method>& method, const char* method_name, int waiting_time);

    /**
     * @brief Prints the output values of the executed model for debugging purposes.
     *
     * @param method The method whose outputs to print.
     */
    void printModelOutput(Result<torch::executor::Method>& method);

    /**
     * @brief Retrieves the output values of the executed model.
     *
     * @param method The method whose outputs to retrieve.
     * @return A vector of output values.
     */
    std::vector<float> getModelOutput(Result<torch::executor::Method>& method);

private:
    /**
     * @brief Private constructor to enforce singleton behavior.
     */
    ModelExecutor();

    /**
     * @brief Planned memory buffers to prevent deallocation during execution.
     */
    std::vector<std::unique_ptr<uint8_t[]>> planned_buffers; 
};

#endif // MODELEXECUTOR_H
