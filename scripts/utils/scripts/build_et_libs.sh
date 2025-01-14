#!/bin/bash

SCRIPT_DIR=$(pwd)
ROOT_DIR=$SCRIPT_DIR/../../
ET_SRC_DIR=$SCRIPT_DIR/../../executorch
MBED_DIR=$SCRIPT_DIR/../../mbed-os

cd $ROOT_DIR
git clone --branch v0.3.0 https://github.com/pytorch/executorch.git
git clone https://github.com/mbed-ce/mbed-os.git

cd $ET_SRC_DIR
git submodule sync
git submodule update --init
python3 -m venv .executorch
source .executorch/bin/activate
./install_requirements.sh
wget https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz
tar -xvf arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz
export PATH=${PATH}:"$(pwd)/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin"

# toolchain must be available in PATH

if hash arm-none-eabi-gcc
then
    echo "toolchain is ready to use"
else 
    echo "toolchain is not available"
    echo $PATH
    exit
fi

patch examples/arm/aot_arm_compiler.py < ../utils/patches/aot_arm_compiler.patch
# Devtools in v040
# https://github.com/pytorch/executorch/blob/25bf93e9babf809db1d7ccc965b0eadf92d9144e/backends/cadence/runtime/executor_main.sh#L17
cmake \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/cmake-out           \
    -DCMAKE_BUILD_TYPE=Release                        \
    -DEXECUTORCH_BUILD_DEVTOOLS=ON                    \
    -DEXECUTORCH_ENABLE_EVENT_TRACER=ON               \
    -DPYTHON_EXECUTABLE="$(which python3)"               \
    -B$(pwd)/cmake-out                                 \
    $(pwd)/cmake-out

cmake --build $(pwd)/cmake-out -j4 --target install --config Release

cmake \
    -DCMAKE_PREFIX_PATH="${PWD}/cmake-out/lib/cmake/ExecuTorch;${PWD}/cmake-out/third-party/gflags" \
    -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_EXECUTABLE="$(which python3)" \
    -B$(pwd)/cmake-out/examples/devtools \
    $(pwd)/examples/devtools

cmake --build $(pwd)/cmake-out/examples/devtools -j4 --config Release

# Basic libraries in v030
cmake                                                 \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/cmake-out           \
    -DEXECUTORCH_BUILD_EXECUTOR_RUNNER=OFF            \
    -DCMAKE_BUILD_TYPE=Release                        \
    -DEXECUTORCH_ENABLE_LOGGING=ON                    \
    -DEXECUTORCH_BUILD_ARM_BAREMETAL=ON               \
    -DEXECUTORCH_BUILD_KERNELS_QUANTIZED=ON           \
    -DEXECUTORCH_BUILD_EXTENSION_RUNNER_UTIL=ON       \
    -DFLATC_EXECUTABLE="$(which flatc)"               \
    -DCMAKE_TOOLCHAIN_FILE=$(pwd)/examples/arm/ethos-u-setup/arm-none-eabi-gcc.cmake        \
    -B$(pwd)/cmake-out                                \
    $(pwd)/cmake-out

cmake --build $(pwd)/cmake-out -j4 --target install --config Release

# OPS List for final CNN 
# Example for multiple not delegated operators: -DEXECUTORCH_SELECT_OPS_LIST="aten::_softmax.out,aten::add.out"

cmake                                                  \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/cmake-out             \
    -DCMAKE_BUILD_TYPE=Release                       \
    -DCMAKE_TOOLCHAIN_FILE=$(pwd)/examples/arm/ethos-u-setup/arm-none-eabi-gcc.cmake  \
    -DTARGET_CPU=cortex-m4  \
    -DEXECUTORCH_SELECT_OPS_LIST="aten::convolution.out,aten::mm.out,aten::add.out,aten::relu.out,aten::linear.out,aten::upsample_bilinear2d.vec_out,aten::_upsample_bilinear2d_aa.out,aten::unsqueeze_copy.out,aten::view_copy.out,aten::max_pool2d_with_indices.out,aten::squeeze_copy.dims_out,executorch_prim::et_view.default,aten::permute_copy.out,aten::addmm.out,aten::_softmax.out"  \
    -B$(pwd)/cmake-out/examples/arm                   \
    $(pwd)/examples/arm

cmake --build $(pwd)/cmake-out/examples/arm 

# Build quntized aot lib
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DEXECUTORCH_BUILD_XNNPACK=OFF \
    -DEXECUTORCH_BUILD_KERNELS_QUANTIZED=ON \
    -DEXECUTORCH_BUILD_KERNELS_QUANTIZED_AOT=ON \
    -DCMAKE_PREFIX_PATH=$(pwd)/executorch_venv/lib/python3.10/site-packages/torch \
    -DPYTHON_EXECUTABLE=python3 \
    -Bcmake-out-aot-lib \
    .

cmake --build $(pwd)/cmake-out-aot-lib -j3 -- quantized_ops_aot_lib

python3 -m examples.arm.aot_arm_compiler --model_name="softmax"

cd examples/arm/executor_runner

cmake -DCMAKE_TOOLCHAIN_FILE=../ethos-u-setup/arm-none-eabi-gcc.cmake \
	-DTARGET_CPU=cortex-m4 \
	-B cmake-out \
	-DET_DIR_PATH:PATH=$(pwd)/../../../         \
	-DET_BUILD_DIR_PATH:PATH=$(pwd)/../../../cmake-out  \
	-DET_PTE_FILE_PATH:PATH="softmax.pte"          \
	-DPYTHON_EXECUTABLE=$(which python3)

cmake --build cmake-out -- -j3 VERBOSE=1 arm_executor_runner

cd $ROOT_DIR
patch executorch/examples/arm/executor_runner/pte_to_header.py < utils/patches/pte_to_header.patch
python3 executorch/examples/arm/executor_runner/pte_to_header.py --pte executorch/add.pte --outdir .

deactivate

cd $MBED_DIR
python3 -m venv venv
source venv/bin/activate
python3 -m pip install -U pyocd
pyocd pack --update
pyocd pack --install stm32wb55rg
deactivate

# In VSC select as pyhton interpreter mbed-os/venv/bin/python3