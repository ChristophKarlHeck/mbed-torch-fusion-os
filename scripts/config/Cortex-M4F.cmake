# Copyright (c) 2020 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
# Change to forse hard FPU
# Sets cpu core options

list(APPEND common_options
        "-mcpu=cortex-m4"
        "-mfpu=fpv4-sp-d16"
        "-mfloat-abi=hard"
    )

function(mbed_set_cpu_core_definitions target)
    target_compile_definitions(${target}
        INTERFACE
            __CORTEX_M4
            ARM_MATH_CM4
            __FPU_PRESENT=1
            __CMSIS_RTOS
    )
endfunction()
