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

# Add sources of static library libexecutorch.a
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

# add_library(executorch STATIC IMPORTED)
# set_property(TARGET executorch PROPERTY IMPORTED_LOCATION
#   "/home/chris/et-tut-3/mbed-torch-fusion-os/executorch/cmake-out/libexecutorch.a")

# Add sources of static library libextension_runner_util.a
set(LIBEXTENSION_RUNNER_UTIL_SRC
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/extension/runner_util/inputs.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/extension/runner_util/inputs_portable.cpp
)

# add_library(extension_runner_util STATIC IMPORTED)
# set_property(TARGET extension_runner_util PROPERTY IMPORTED_LOCATION
#   "/home/chris/et-tut-3/mbed-torch-fusion-os/executorch/cmake-out/extension/runner_util/libextension_runner_util.a")

# Add sources of static library libportable_kernels.a
set(LIBPORTABLE_KERNELS
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_abs.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_acos.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_acosh.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_add.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_addmm.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_alias_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_allclose.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_amax.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_amin.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_any.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_arange.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_argmax.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_argmin.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_as_strided_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_asin.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_asinh.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_atan.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_atan2.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_atanh.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_avg_pool2d.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_bitwise_and.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_bitwise_not.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_bitwise_or.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_bitwise_xor.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_bmm.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_cat.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_cdist_forward.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_ceil.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_clamp.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_clone.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_constant_pad_nd.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_convolution.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_cos.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_cosh.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_cumsum.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_detach_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_diagonal_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_div.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_embedding.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_empty.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_eq.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_erf.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_exp.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_expand_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_expm1.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_fill.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_flip.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_floor.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_floor_divide.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_fmod.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_full.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_full_like.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_ge.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_gelu.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_glu.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_gt.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_hardtanh.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_index.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_index_put.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_index_select.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_isinf.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_isnan.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_le.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_leaky_relu.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_lift_fresh_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_linear_scratch_example.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_log.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_log10.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_log1p.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_log2.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_log_softmax.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_logical_and.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_logical_not.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_logical_or.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_logical_xor.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_logit.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_lt.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_masked_fill.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_max.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_max_pool2d_with_indices.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_maximum.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_mean.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_min.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_minimum.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_mm.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_mul.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_native_batch_norm.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_native_group_norm.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_native_layer_norm.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_ne.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_neg.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_nonzero.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_ones.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_pdist_forward.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_permute_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_pixel_shuffle.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_pow.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_prod.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_reciprocal.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_reflection_pad1d.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_reflection_pad2d.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_reflection_pad3d.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_relu.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_remainder.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_repeat.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_replication_pad1d.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_replication_pad2d.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_replication_pad3d.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_roll.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_round.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_rsqrt.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_rsub.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_scalar_tensor.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_scatter_add.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_select_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_select_scatter.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_sigmoid.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_sign.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_sin.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_sinh.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_slice_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_slice_scatter.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_softmax.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_split_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_split_with_sizes_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_sqrt.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_squeeze_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_stack.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_sub.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_sum.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_t_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_tan.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_tanh.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_to_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_transpose_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_tril.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_trunc.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_unbind_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_unsqueeze_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_var.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_view_copy.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_where.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/op_zeros.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/pattern/binary_ufunc_realb_realb_to_realb_logical.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/pattern/unary_ufunc_realh.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/pattern/unary_ufunc_realhb_to_bool.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/pattern/unary_ufunc_realhb_to_floath.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/util/activation_ops_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/util/advanced_index_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/util/broadcast_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/util/copy_ops_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/util/distance_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/util/index_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/util/kernel_ops_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/util/matmul_ops_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/util/normalization_ops_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/util/padding_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/util/reduce_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/kernels/portable/cpu/util/repeat_util.cpp   
)

# add_library(portable_kernels STATIC IMPORTED)
# set_property(TARGET portable_kernels PROPERTY IMPORTED_LOCATION
#   "/home/chris/et-tut-3/mbed-torch-fusion-os/executorch/cmake-out/kernels/portable/libportable_kernels.a")

# Add sources of static library libarm_portable_ops_lib.a
set(LIBARM_PORTABLE_OPS_LIB
     ${CMAKE_CURRENT_SOURCE_DIR}/config/executorch/cmake-out/examples/arm/RegisterCodegenUnboxedKernelsEverything.cpp
)

# add_library(portable_ops_lib STATIC IMPORTED)
# set_property(TARGET portable_ops_lib PROPERTY IMPORTED_LOCATION
#   "/home/chris/et-tut-3/mbed-torch-fusion-os/executorch/cmake-out/kernels/portable/libportable_ops_lib.a")

### ALL SOURCES IN THIS FILE###
include_directories(
     ${CMAKE_CURRENT_SOURCE_DIR}/config/executorch/cmake-out/schema/include
     ${CMAKE_CURRENT_SOURCE_DIR}/executorch/third-party/flatbuffers/include
)

add_executable(SoftMaxTorchRunner
     main.cpp
     ${LIBEXECUTORCH_SRC}
     ${LIBEXTENSION_RUNNER_UTIL_SRC}
     ${LIBPORTABLE_KERNELS}
     ${LIBARM_PORTABLE_OPS_LIB}
)

# add_executable(SoftMaxTorchRunner
#     main.cpp
# )

target_link_libraries(SoftMaxTorchRunner mbed-os)

# target_link_libraries(SoftMaxTorchRunner PUBLIC
#      mbed-os # Can also link to mbed-baremetal here
#      "-Wl,--whole-archive"
#      extension_runner_util
#      executorch
#      portable_kernels
#      portable_ops_lib
#      "-Wl,--no-whole-archive"
#      ) 

###GENRAL###
target_include_directories(SoftMaxTorchRunner
     PUBLIC 
          ${CMAKE_CURRENT_SOURCE_DIR}
)

###GENRAL####
mbed_set_post_build(SoftMaxTorchRunner) # Must call this for each target to set up bin file creation, code upload, etc


###GENRAL###
mbed_finalize_build() # Make sure this is the last line of the top-level buildscript
