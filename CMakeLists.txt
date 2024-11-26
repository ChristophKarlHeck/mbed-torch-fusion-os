#
# Mbed Torch Fusion OS Project
#
cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19)

# Enable OPENOCD upload method for NUCLEO_WB55RG
file(COPY ${CMAKE_SOURCE_DIR}/libs/config/mbed-os/NUCLEO_WB55RG.cmake
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
     ${CMAKE_CURRENT_SOURCE_DIR}/src/model_executor/ModelExecutor.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/src/adc/AD7124.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/src/interfaces/ReadingQueue.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/src/interfaces/SendingQueue.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Conversion.cpp
)

add_executable(SoftMaxTorchRunner ${SOURCES})

#target_link_libraries(SoftMaxTorchRunner mbed-os)

target_link_libraries(SoftMaxTorchRunner PUBLIC
     mbed-os # Can also link to mbed-baremetal here
     #mbed-ble
     "-Wl,--whole-archive"
     ${CMAKE_CURRENT_SOURCE_DIR}/libs/executorch/cmake-out/extension/runner_util/libextension_runner_util.a
     ${CMAKE_CURRENT_SOURCE_DIR}/libs/executorch/cmake-out/libexecutorch.a
     ${CMAKE_CURRENT_SOURCE_DIR}/libs/executorch/cmake-out/libexecutorch_no_prim_ops.a
     ${CMAKE_CURRENT_SOURCE_DIR}/libs/executorch/cmake-out/kernels/portable/libportable_kernels.a
     ${CMAKE_CURRENT_SOURCE_DIR}/libs/executorch/cmake-out/examples/arm/libarm_portable_ops_lib.a
     "-Wl,--no-whole-archive"
     ) 

###GENRAL###
target_include_directories(SoftMaxTorchRunner
     PUBLIC 
          ${CMAKE_CURRENT_SOURCE_DIR}/include #need it for executorch includes and logger.h
          ${CMAKE_CURRENT_SOURCE_DIR}/include/model_executor
          ${CMAKE_CURRENT_SOURCE_DIR}/libs
          ${CMAKE_CURRENT_SOURCE_DIR}/include/adc
          ${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces
          ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
          #${CMAKE_CURRENT_SOURCE_DIR}/BLE_Service // needs to be split in .h and .cpp file
         
)

#add_dependencies(SoftMaxTorchRunner gen_model_header)

###GENRAL####
mbed_set_post_build(SoftMaxTorchRunner) # Must call this for each target to set up bin file creation, code upload, etc


###GENRAL###
mbed_finalize_build() # Make sure this is the last line of the top-level buildscript