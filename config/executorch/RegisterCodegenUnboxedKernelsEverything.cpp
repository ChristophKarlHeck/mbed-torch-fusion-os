/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <executorch/runtime/core/evalue.h>
#include <executorch/runtime/core/exec_aten/exec_aten.h>
#include <executorch/runtime/kernel/operator_registry.h>
#include <executorch/runtime/platform/profiler.h>
#include "NativeFunctions.h" // Generated Function import headers
// @generated by torchgen/gen.py from RegisterCodegenUnboxedKernels.cpp

// NOTE [Sharded File]: This file is generated in a sharded fashion to speed up
// incremental rebuilds. See the comment at the top of
// templates/VariableType.cpp for an analogous, in-depth discussion.
//
// Generated by tools/jit/gen_unboxing.py. This file registers all ATen ops into
// JIT op registry instead of c10 dispatcher. JIT op registry only takes boxed
// kernels, so we are calling unboxing functions in UnboxingFunctions.h to cast
// arguments into C++ types (instead of IValue) and delegate to unboxed kernels.
using KernelArrayRef = ::torch::executor::ArrayRef<::torch::executor::Kernel>;
namespace torch {
namespace executor {
namespace function {
namespace {

static Kernel kernels_to_register[] = {
    
    Kernel(
        "aten::_softmax.out",
        [](torch::executor::KernelRuntimeContext & context, EValue** stack) {
            EValue& self = *stack[0];
    	EValue& dim = *stack[1];
    	EValue& half_to_float = *stack[2];
    	EValue& out = *stack[3];
    	const torch::executor::Tensor & self_base = self.to<torch::executor::Tensor>();
    	int64_t dim_base = dim.to<int64_t>();
    	bool half_to_float_base = half_to_float.to<bool>();
    	torch::executor::Tensor & out_base = out.to<torch::executor::Tensor>();
    
            internal::EventTracerProfileScope event_tracer_scope(context.internal_event_tracer(), "native_call__softmax.out");
            EXECUTORCH_SCOPE_PROF("native_call__softmax.out");
            torch::executor::native::softmax_out(context, self_base, dim_base, half_to_float_base, out_base);
            internal::event_tracer_log_evalue(context.internal_event_tracer(), *stack[3]);
    
            
        }
    ), // Generated kernels
};

// Explicitly convert to ArrayRef, so that the API can take an empty C array of
// Kernels.
static KernelArrayRef kernel_array_ref(
    kernels_to_register,
    kernels_to_register + sizeof(kernels_to_register) / sizeof(Kernel));

// Return value not used. Keep the static variable assignment to register
// kernels in static initialization time.
static auto success_with_kernel_reg = register_kernels(kernel_array_ref);
} // namespace
} // namespace function
} // namespace executor
} // namespace torch