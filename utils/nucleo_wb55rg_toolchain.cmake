# the name of the target operating system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)

# Specify the cross compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-as)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)

# adjust the default behavior of the FIND_XXX() commands:
# search for headers and libraries in the target environment,
# search for programs in the host environment
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=cortex-m4 -mthumb")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T/home/chris/et-tut-3/mbed-torch-fusion-os/mbed-os/targets/TARGET_STM/TARGET_STM32WB/TARGET_STM32WB55xG/TOOLCHAIN_GCC_ARM/stm32wb55xg.ld")

# Specify any other necessary flags or configurations
add_compile_definitions(
    "$<$<NOT:$<CONFIG:DEBUG>>:NDEBUG>")

add_link_options(
    "-Wl,--start-group"
        "-lstdc++"
        "-lsupc++"
        "-lm"
        "-lc"
        "-lgcc"
        "-lnosys"
    "-Wl,--end-group"
    "-specs=nosys.specs"
    "-Wl,--cref")