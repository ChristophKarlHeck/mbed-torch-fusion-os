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
# Change to 4 inputs 
python3 -m examples.arm.aot_arm_compiler --model_name="add"

cmake                                                 \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/cmake-out           \
    -DEXECUTORCH_BUILD_EXECUTOR_RUNNER=OFF            \
    -DCMAKE_BUILD_TYPE=Debug                        \
    -DEXECUTORCH_ENABLE_LOGGING=OFF                    \
    -DEXECUTORCH_BUILD_ARM_BAREMETAL=ON               \
    -DEXECUTORCH_BUILD_EXTENSION_RUNNER_UTIL=ON       \
    -DFLATC_EXECUTABLE="$(which flatc)"               \
    -DCMAKE_TOOLCHAIN_FILE=$(pwd)/examples/arm/ethos-u-setup/arm-none-eabi-gcc.cmake        \
    -B$(pwd)/cmake-out                                \
    $(pwd)/cmake-out

cmake --build $(pwd)/cmake-out -j4 --target install --config Debug

# Example for multiple not delegated operators: -DEXECUTORCH_SELECT_OPS_LIST="aten::_softmax.out,aten::add.out"

cmake                                                  \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/cmake-out             \
    -DCMAKE_BUILD_TYPE=Debug                         \
    -DCMAKE_TOOLCHAIN_FILE=$(pwd)/examples/arm/ethos-u-setup/arm-none-eabi-gcc.cmake  \
    -DTARGET_CPU=cortex-m4  \
    -DEXECUTORCH_SELECT_OPS_LIST="aten::_softmax.out"  \
    -B$(pwd)/cmake-out/examples/arm                   \
    $(pwd)/examples/arm

cmake --build $(pwd)/cmake-out/examples/arm --config Debug

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