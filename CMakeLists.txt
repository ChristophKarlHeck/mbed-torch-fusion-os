#
# Mbed Torch Fusion OS Project
#
cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19)

# Initialize Mbed OS build system. 
# Note: This block must be before the project() call.
set(MBED_APP_JSON_PATH mbed_app.json5)
# set(CUSTOM_TARGETS_JSON_PATH custom_targets.json) # If you need a custom target, use this line to specify the custom_targets.json

include(mbed-os/tools/cmake/app.cmake) # Load Mbed CE toolchain file and basic build system

# If you need any custom upload method configuration for your target, do that here

add_subdirectory(mbed-os) # Load Mbed OS build targets.  Must be added before any other subdirectories

project(MbedTorchFusionOS) # TODO: change this to your project name

add_executable(SoftMaxTorchRunner main.cpp)
target_link_libraries(SoftMaxTorchRunner mbed-os) # Can also link to mbed-baremetal here
mbed_set_post_build(SoftMaxTorchRunner) # Must call this for each target to set up bin file creation, code upload, etc

# Copy the corrct mbed_config.cmake for NUCLEO_WB55RG to the build directory
add_custom_command(TARGET SoftMaxTorchRunner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/mbed_config.cmake
            ${CMAKE_BINARY_DIR}/mbed_config.cmake
    COMMENT "Copying mbed_config.cmake to build directory..."
)

mbed_finalize_build() # Make sure this is the last line of the top-level buildscript