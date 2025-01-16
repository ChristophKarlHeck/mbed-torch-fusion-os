#
# Mbed Torch Fusion OS Project
#
cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19)

# Enable OPENOCD upload method for NUCLEO_WB55RG
file(COPY ${CMAKE_SOURCE_DIR}/scripts/config/NUCLEO_WB55RG.cmake
     DESTINATION ${CMAKE_SOURCE_DIR}/mbed-os/targets/upload_method_cfg)


# Initialize Mbed OS build system. 
# Note: This block must be before the project() call.
set(MBED_APP_JSON_PATH ${CMAKE_SOURCE_DIR}/mbed_app.json5)
# set(CUSTOM_TARGETS_JSON_PATH custom_targets.json) # If you need a custom target, use this line to specify the custom_targets.json

include(mbed-os/tools/cmake/app.cmake) # Load Mbed CE toolchain file and basic build system

# If you need any custom upload method configuration for your target, do that here

add_subdirectory(mbed-os) # Load Mbed OS build targets.  Must be added before any other subdirectories

project(MbedTorchFusionOS CXX) # TODO: change this to your project name

set(SOURCES 
     ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/mbed_stats_wrapper.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/src/model_executor/ModelExecutor.cpp
)

add_executable(MbedTorchFusionOS ${SOURCES})

target_link_libraries(MbedTorchFusionOS PUBLIC
     mbed-os # Can also link to mbed-baremetal here
     /home/chris/executorch_v030/executorch/cmake-out/lib/libextension_runner_util.a
     "-Wl,--whole-archive"
     /home/chris/executorch_v030/executorch/cmake-out/lib/libexecutorch.a
     /home/chris/executorch_v030/executorch/cmake-out/lib/libexecutorch_no_prim_ops.a
     /home/chris/executorch_v030/executorch/cmake-out/kernels/quantized/libquantized_ops_lib.a
     /home/chris/executorch_v030/executorch/cmake-out/examples/arm/libarm_portable_ops_lib.a
     /home/chris/executorch_v030/executorch/cmake-out/kernels/quantized/libquantized_kernels.a
     /home/chris/executorch_v030/executorch/cmake-out/lib/libportable_kernels.a
     "-Wl,--no-whole-archive"
     ) 

###GENRAL###
target_include_directories(MbedTorchFusionOS
     PUBLIC
          ${CMAKE_CURRENT_SOURCE_DIR}/include
          /home/chris/executorch_v030/ #for executorch headers
          ${CMAKE_CURRENT_SOURCE_DIR}/models
)

#add_dependencies(MbedTorchFusionOS gen_model_header)

###GENRAL####
mbed_set_post_build(MbedTorchFusionOS) # Must call this for each target to set up bin file creation, code upload, etc


###GENRAL###
mbed_finalize_build() # Make sure this is the last line of the top-level buildscript