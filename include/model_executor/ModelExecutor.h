#ifndef MODEL_EXECUTOR_H
#define MODEL_EXECUTOR_H

#include <vector>
#include <memory>
#include <stdint.h>

class ModelExecutor {
public:
    // Singleton access
    static ModelExecutor& getInstance(size_t pool_size = 512);

    // Run the model with the provided inputs
    std::vector<float> run_model(std::vector<float> feature_vector);

    // Delete copy constructor and assignment operator to enforce singleton pattern
    ModelExecutor(const ModelExecutor&) = delete;
    ModelExecutor& operator=(const ModelExecutor&) = delete;

private:
    // Private constructor for the singleton
    ModelExecutor(size_t pool_size);

    // Private destructor
    ~ModelExecutor();

    uint8_t* m_method_allocator_pool;
    size_t m_allocator_pool_size;
};

#endif // MODEL_EXECUTOR_H