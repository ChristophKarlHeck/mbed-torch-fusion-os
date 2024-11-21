#include <stdio.h>
#include <memory>
#include <vector>

#include <executorch/extension/data_loader/buffer_data_loader.h>
#include <executorch/extension/runner_util/inputs.h>
#include <executorch/runtime/executor/program.h>
#include <executorch/runtime/platform/log.h>
#include <executorch/runtime/platform/platform.h>
#include <executorch/runtime/platform/runtime.h>

using namespace exec_aten;
using namespace std;
using torch::executor::Error;
using torch::executor::Result;



class ModelExecutor 
{
    public:
        ModelExecutor(void);

        void initRuntime(void);

        Result<torch::executor::Program> loadModelBuffer(void);

        const char* getMethodName(Result<torch::executor::Program>& program);

        Result<torch::executor::MethodMeta> getMethodMeta(
            Result<torch::executor::Program> &program,
            const char* method_name);

        torch::executor::MemoryAllocator getMemoryAllocator(void);

        std::vector<torch::executor::Span<uint8_t>> setUpPlannedBuffer(
            Result<torch::executor::Program> &program,
            Result<torch::executor::MethodMeta>& method_meta);

        Result<torch::executor::Method> loadMethod(
            Result<torch::executor::Program>& program,
            torch::executor::MemoryAllocator& method_allocator,
            std::vector<torch::executor::Span<uint8_t>>& planned_spans,
            const char* method_name);

        unsigned int getNumberOfInputValues(Result<torch::executor::Method>& method);

        void prepareInputs(Result<torch::executor::Method>& method, const char* method_name);

        void setModelInput(Result<torch::executor::Method>& method, std::vector<float>& inputs);

        void printModelInput(Result<torch::executor::Method>& method);

        void executeModel(Result<torch::executor::Method>& method, const char* method_name);

        void printModelOutput(Result<torch::executor::Method>& method);

        std::vector<float> getModelOutput(Result<torch::executor::Method>& method);

    private:

        // Needed, otherwise planned buffers will be deallocated and makes planned_spans invalid.
        std::vector<std::unique_ptr<uint8_t[]>> planned_buffers; 
};