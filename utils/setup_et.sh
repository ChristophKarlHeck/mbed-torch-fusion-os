#!/bin/bash

SCRIPT_DIR=$(pwd)
ET_SRC_DIR=$SCRIPT_DIR/../executorch
ET_BUILD_DIR=$ET_SRC_DIR/cmake-out
ET_VENV=$ET_SRC_DIR/.executorch

cd $ET_SRC_DIR
git submodule sync
git submodule update --init
cd third-party/flatbuffers
git checkout 6ff9e90e7e399f3977e99a315856b57c8afe5b4d # flatbuffers version 24.3.7
git pull origin master
cd $ET_SRC_DIR

if [ -d "$ET_VENV" ] 
then
	echo "venv exists"
else
    echo venv does not exists
	python3 -m venv .executorch
fi

source .executorch/bin/activate

# Download toolchain and load toolchain into Path of venv
# https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
source ~/.bash_profile

# Check if toolchain is available
if hash arm-none-eabi-gcc
then
    echo "toolchain is ready to use"
else 
    echo "toolchain is not available"
    echo $PATH
    exit
fi

# Fix error: Preparing metadata (pyproject.toml) did not run successfully.
pip3 cache purge
pip3 install meson
python3 -m pip install --upgrade pip
pip3 install importlib-metadata
python3 -m pip install --upgrade setuptools
pip3 install subprocess.run
pip3 install wheel setuptools pip --upgrade
pip3 install toml
pip3 install numpy --use-deprecated=legacy-resolver
pip3 install setuptools-metadata
# Fix error end #

# Install requirements
pip3 install zstd
pip3 install cmake
./install_requirements.sh

export PATH="$(pwd)/third-party/flatbuffers/cmake-out:${PATH}"
"$(pwd)/build/install_flatc.sh"

# Download buck2
if [ -e "$ET_SRC_DIR/buck2-x86_64-unknown-linux-gnu.zst" ]
then
	echo "buck2 archive ready"
else
	wget https://github.com/facebook/buck2/releases/download/latest/buck2-x86_64-unknown-linux-gnu.zst
fi

zstd -cdq buck2-x86_64-unknown-linux-gnu.zst > /tmp/buck2 && chmod +x /tmp/buck2
/tmp/buck2 clean
/tmp/buck2 kill

cd third-party/flatbuffers
git checkout 129ef422e8a4e89d87a7216a865602673a6d0bf3
git pull origin master
cd $ET_SRC_DIR

MBED_CONFIG_CMAKE=$SCRIPT_DIR../build/mbed_config.cmake #suits to Hardware

#TARGETS PATCH NECESSARY

set -x

# build dir pf mbed os has to generated before because of MBED_CONFIG_CMAKE

cmake                                                 \
    -DBUCK2=/tmp/buck2                                \
    -DCMAKE_INSTALL_PREFIX=${ET_BUILD_DIR}            \
    -DEXECUTORCH_BUILD_EXECUTOR_RUNNER=OFF            \
    -DCMAKE_BUILD_TYPE=Release                        \
    -DEXECUTORCH_ENABLE_LOGGING=ON                    \
    -DEXECUTORCH_BUILD_ARM_BAREMETAL=ON               \
    -DEXECUTORCH_BUILD_EXTENSION_RUNNER_UTIL=ON       \
    -DFLATC_EXECUTABLE="$(which flatc)"               \
    -DCMAKE_TOOLCHAIN_FILE="${MBED_CONFIG_CMAKE}"  \
    -B${ET_BUILD_DIR}                                 \
    "${ET_SRC_DIR}"

###TEST###
cmake                                                 \
    -DBUCK2=/tmp/buck2                                \
    -DCMAKE_INSTALL_PREFIX=/home/chris/et-tut-3/mbed-torch-fusion-os/executorch/cmake-out            \
    -DEXECUTORCH_BUILD_EXECUTOR_RUNNER=OFF            \
    -DCMAKE_BUILD_TYPE=Release                        \
    -DEXECUTORCH_ENABLE_LOGGING=ON                    \
    -DEXECUTORCH_BUILD_ARM_BAREMETAL=ON               \
    -DEXECUTORCH_BUILD_EXTENSION_RUNNER_UTIL=ON       \
    -DFLATC_EXECUTABLE="$(which flatc)"               \
    -DCMAKE_TOOLCHAIN_FILE=/home/chris/et-tut-3/mbed-torch-fusion-os/build/mbed_config.cmake \
    -B/home/chris/et-tut-3/mbed-torch-fusion-os/executorch/cmake-out                                \
    /home/chris/et-tut-3/mbed-torch-fusion-os/executorch

cmake --build ${ET_BUILD_DIR} -j4 --target install --config Release

###TEST###
cmake --build /home/chris/et-tut-3/mbed-torch-fusion-os/executorch/cmake-out -j4 --target install --config Release

cmake                                                  \
    -DCMAKE_INSTALL_PREFIX=${ET_BUILD_DIR}             \
    -DCMAKE_BUILD_TYPE=Release                         \
    -DCMAKE_TOOLCHAIN_FILE="${MBED_CONFIG_CMAKE}"   \
    -DEXECUTORCH_SELECT_OPS_LIST="aten::_softmax.out"  \
    -B"${ET_BUILD_DIR}"/examples/arm                   \
    "${ET_SRC_DIR}"/examples/arm 

###TEST###
cmake                                                  \
    -DCMAKE_INSTALL_PREFIX=/home/chris/et-tut-3/mbed-torch-fusion-os/executorch/cmake-out             \
    -DCMAKE_BUILD_TYPE=Release                         \
    -DCMAKE_TOOLCHAIN_FILE=/home/chris/et-tut-3/mbed-torch-fusion-os/build/mbed_config.cmake   \
    -DEXECUTORCH_SELECT_OPS_LIST="aten::_softmax.out"  \
    -B/home/chris/et-tut-3/mbed-torch-fusion-os/executorch/cmake-out/examples/arm                   \
    /home/chris/et-tut-3/mbed-torch-fusion-os/executorch/examples/arm 

cmake --build ${et_build_dir}/examples/arm --config Release

###TEST###
cmake --build /home/chris/et-tut-3/mbed-torch-fusion-os/executorch/cmake-out/examples/arm --config Release

set +x

cd "${ET_BUILD_DIR}"
echo "Generated static libraries for ExecuTorch:"
echo "$(pwd)"
find . -name "*.a" -exec ls -al {} \;