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



class ModelExecutor {
public:
    ModelExecutor(void);

    void initRuntime(void);

    Result<torch::executor::Program> loadModelBuffer(void);

    void runModel(Result<torch::executor::Program>& program);

private:

};