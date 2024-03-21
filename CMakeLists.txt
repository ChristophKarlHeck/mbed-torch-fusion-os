#
# Mbed Torch Fusion OS Project
#
cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19)

# Enable OPENOCD upload method for NUCLEO_WB55RG
file(COPY ${CMAKE_SOURCE_DIR}/config/mbed-os/NUCLEO_WB55RG.cmake
     DESTINATION ${CMAKE_SOURCE_DIR}/mbed-os/targets/upload_method_cfg)

# Initialize Mbed OS build system. 
# Note: This block must be before the project() call.
set(MBED_APP_JSON_PATH mbed_app.json5)
# set(CUSTOM_TARGETS_JSON_PATH custom_targets.json) # If you need a custom target, use this line to specify the custom_targets.json

include(mbed-os/tools/cmake/app.cmake) # Load Mbed CE toolchain file and basic build system

# If you need any custom upload method configuration for your target, do that here

add_subdirectory(mbed-os) # Load Mbed OS build targets.  Must be added before any other subdirectories

project(MbedTorchFusionOS) # TODO: change this to your project name

# Add sources of libexecutorch.a
set(LIBEXECUTORCH_SRC
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/prim_ops/et_copy_index.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/prim_ops/register_prim_ops.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/backend/interface.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/core/evalue.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/core/exec_aten/util/tensor_util_portable.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/core/portable_type/tensor_impl.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/executor/method.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/executor/method_meta.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/executor/program.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/executor/tensor_parser_exec_aten.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/executor/tensor_parser_portable.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/kernel/operator_registry.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/platform/abort.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/platform/log.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/platform/profiler.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/platform/runtime.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/runtime/platform/target/Posix.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/schema/extended_header.cpp
)


add_executable(SoftMaxTorchRunner
                    main.cpp
                    ${LIBEXECUTORCH_SRC}
               )

target_link_libraries(SoftMaxTorchRunner mbed-os) # Can also link to mbed-baremetal here

target_include_directories(SoftMaxTorchRunner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

mbed_set_post_build(SoftMaxTorchRunner) # Must call this for each target to set up bin file creation, code upload, etc

mbed_finalize_build() # Make sure this is the last line of the top-level buildscript
