#include "ModelExecutor.h"
#include "model_pte.h"

//__attribute__((section(".sram.data"), aligned(16)))
uint8_t method_allocator_pool[4 * 1024U];

//void et_pal_init(void) {}

__ET_NORETURN void et_pal_abort(void)
{
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
    __ET_UNUSED size_t length)
{
  fprintf(stderr, "%c executorch:%s:%zu] %s\n", level, filename, line, message);
}

ModelExecutor::ModelExecutor(void)
{
    //printf("hi");
}

void ModelExecutor::initRuntime(void)
{
    torch::executor::runtime_init();
}

Result<torch::executor::Program> ModelExecutor::loadModelBuffer(void)
{
    auto loader = torch::executor::util::BufferDataLoader(model_pte, sizeof(model_pte));
    //ET_LOG(Info, "Model PTE file loaded. Size: %lu bytes.", sizeof(model_pte));
    Result<torch::executor::Program> program = torch::executor::Program::load(&loader);
    if (!program.ok()) {
        ET_LOG(
            Info,
            "Program loading failed @ 0x%p: 0x%" PRIx32,
            model_pte,
            program.error());
    }

    ET_LOG(Info, "Model buffer loaded, has %zu methods.", program->num_methods());

    return program;
}

const char* ModelExecutor::getMethodName(Result<torch::executor::Program> &program)
{
    const char* method_name = nullptr;
    {
        const auto method_name_result = program->get_method_name(0);
        ET_CHECK_MSG(method_name_result.ok(), "Program has no methods");
        method_name = *method_name_result;
    }
    ET_LOG(Info, "Running method %s.", method_name);

    return method_name;
}

Result<torch::executor::MethodMeta> ModelExecutor::getMethodMeta(
    Result<torch::executor::Program> &program,
    const char* method_name)
{

    Result<torch::executor::MethodMeta> method_meta =
        program->method_meta(method_name);
    if (!method_meta.ok()) {
        ET_LOG(
            Info,
            "Failed to get method_meta for %s: 0x%x",
            method_name,
            (unsigned int)method_meta.error());
        return method_meta;
    }

    ET_LOG(Info, "Fetched method meta.");
    return method_meta;

}

torch::executor::MemoryAllocator ModelExecutor::getMemoryAllocator(void)
{
    // Pool size is defined at the beginning of this file and can be adjusted
    torch::executor::MemoryAllocator method_allocator{
        torch::executor::MemoryAllocator(
            sizeof(method_allocator_pool), method_allocator_pool)}; 

    ET_LOG(Info, "Got method allocator.");
    return method_allocator;
}

std::vector<torch::executor::Span<uint8_t>> ModelExecutor::setUpPlannedBuffer(
    Result<torch::executor::Program> &program,
    Result<torch::executor::MethodMeta>& method_meta)
{


    planned_buffers.clear(); // Owns the memory
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

    return planned_spans;
}

Result<torch::executor::Method> ModelExecutor::loadMethod(
    Result<torch::executor::Program>& program,
    torch::executor::MemoryAllocator& method_allocator,
    std::vector<torch::executor::Span<uint8_t>>& planned_spans,
    const char* method_name)
{
    
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

    return method;
}

void ModelExecutor::prepareInputs(
    Result<torch::executor::Method>& method,
    const char* method_name)
{
    ET_LOG(Info, "Preparing inputs...");
    auto inputs = torch::executor::util::prepare_input_tensors(*method);
    if (!inputs.ok()) {
        ET_LOG(
            Info,
            "Preparing inputs tensors for method %s failed with status 0x%" PRIx32,
            method_name,
            inputs.error());
    }
}

void ModelExecutor::setModelInput(Result<torch::executor::Method>& method, std::vector<float>& inputs)
{
    const torch::executor::EValue input_original = method->get_input(0);
    Tensor tensor = input_original.payload.as_tensor;
    float* data = input_original.payload.as_tensor.mutable_data_ptr<float>();

    // Change input
    int j;
    for(j = 0; j < tensor.numel(); ++j){
        data[j] = inputs[j]; 
    }

    // Set input 
    method->set_input(input_original,0);
    
}

void ModelExecutor::printModelInput(Result<torch::executor::Method>& method)
{
    size_t input_size = method->inputs_size();
    const torch::executor::EValue input_new = method->get_input(0);
    for (unsigned i = 0; i < input_size; ++i) {
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

}

void ModelExecutor::executeModel(
    Result<torch::executor::Method>& method,
    const char* method_name)
{
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

}

void ModelExecutor::printModelOutput(Result<torch::executor::Method>& method)
{
    
    std::vector<torch::executor::EValue> outputs(method->outputs_size());
    ET_LOG(Info, "%zu outputs: ", outputs.size());
    Error status = method->get_outputs(outputs.data(), outputs.size());
    ET_CHECK(status == Error::Ok);
    for (unsigned int i = 0; i < outputs.size(); ++i) {
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
}

std::vector<float> ModelExecutor::getModelOutput(Result<torch::executor::Method>& method)
{
    std::vector<torch::executor::EValue> outputs(method->outputs_size());
    Error status = method->get_outputs(outputs.data(), outputs.size());
    ET_CHECK(status == Error::Ok); // Ensure we check for success

    std::vector<float> result; // Vector to hold the output values

    for (size_t i = 0; i < outputs.size(); ++i) {
        Tensor t = outputs[i].toTensor();
        for (int j = 0; j < t.numel(); ++j) {
            if (t.scalar_type() == ScalarType::Int) {
                // If the tensor is of type int, convert it to float and add to the results
                result.push_back(static_cast<float>(outputs[i].toTensor().const_data_ptr<int>()[j]));
            } else {
                // If the tensor is of type float, add it directly to the results
                result.push_back(outputs[i].toTensor().const_data_ptr<float>()[j]);
            }
        }
    }

    return result; // Return the vector containing all outputs
}