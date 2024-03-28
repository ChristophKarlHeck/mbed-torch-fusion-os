# Mbed Torch Fusion OS
MbedTorch Fusion OS: Seamlessly merges Mbed OS with Torch ML models for the P-Nucleo-WB55RG on Arm Cortex M4. Empowering edge computing with efficient ML deployment.

## 1. Set up the obligatory versions
> git clone https://github.com/ChristophKarlHeck/mbed-torch-fusion-os.git

> cd mbed-torch-fusion-os

> git submodule sync

> git submodule update --init

> cd executorch

> git checkout ceb1f1d05ceab420644a3633264f13547dc02411

> git submodule sync

> git submodule update --init

> cd third-party/flatbuffers && git checkout 6ff9e90e7e399f3977e99a315856b57c8afe5b4d

> cd ../../../mbed-os

> git checkout 862f46233c3d074048b8f4003aceea4b0d254694

> cd ..

> python3 --version (Python 3.10.12)

## 2. Set up ExecuTorch

> cd executorch

> python3 -m venv .executorch

> source .executorch/bin/activate

### 2.1 # Fix error: Preparing metadata (pyproject.toml) did not run successfully.

> pip3 cache purge

> pip3 install meson==1.3.2

> python3 -m pip install --upgrade pip

> pip3 install importlib-metadata==7.0.2

> python3 -m pip install setuptools==69.1.1

> pip3 install subprocess.run==0.0.8

> pip3 install wheel==0.42.0 setuptools pip --upgrade

> pip3 install toml==0.10.2

> pip3 install numpy==1.26.4 --use-deprecated=legacy-resolver

> pip install setuptools-metadata==0.1.5

### End of fix

> pip3 install tomli==2.0.1

> pip3 install cmake==3.28.3

> patch build/install_flatc.sh < ../utils/patches/install_flatc.patch

> ./install_requirements.sh

> export PATH="$(pwd)/third-party/flatbuffers/cmake-out:${PATH}"

> "$(pwd)/build/install_flatc.sh"

> wget https://github.com/facebook/buck2/releases/download/2024-03-01/buck2-x86_64-unknown-linux-gnu.zst

> zstd -cdq buck2-x86_64-unknown-linux-gnu.zst > /tmp/buck2 && chmod +x /tmp/buck2

> cd ..

> wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2

> tar -xvf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2

> export PATH="$(pwd)/gcc-arm-none-eabi-10.3-2021.10/bin:${PATH}"

> hash arm-none-eabi-gcc

> cd executorch

> patch --verbose --ignore-whitespace --fuzz 3 examples/arm/aot_arm_compiler.py < ../utils/patches/aot_arm_compiler.patch

> python3 -m examples.arm.aot_arm_compiler --model_name="softmax"

> patch examples/arm/ethos-u-setup/arm-none-eabi-gcc.cmake < ../utils/patches/cmake_file_m4.patch

> patch kernels/portable/targets.bzl ../utils/patches/targets.patch

> cmake    \
>    -DBUCK2=                                                          \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/cmake-out            \
    -DEXECUTORCH_BUILD_EXECUTOR_RUNNER=OFF            \
    -DCMAKE_BUILD_TYPE=Release                        \
    -DEXECUTORCH_ENABLE_LOGGING=ON                    \
    -DEXECUTORCH_BUILD_ARM_BAREMETAL=ON               \
    -DEXECUTORCH_BUILD_EXTENSION_RUNNER_UTIL=ON       \
    -DFLATC_EXECUTABLE="$(which flatc)"               \
    -DCMAKE_TOOLCHAIN_FILE=$(pwd)/examples/arm/ethos-u-setup/arm-none-eabi-gcc.cmake  \
    -B$(pwd)/cmake-out                                \
    $(pwd)


> cmake --build $(pwd)/cmake-out -j4 --target install --config Release

>cmake                                                  \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/cmake-out             \
    -DCMAKE_BUILD_TYPE=Release                         \
    -DCMAKE_TOOLCHAIN_FILE=$(pwd)/examples/arm/ethos-u-setup/arm-none-eabi-gcc.cmake  \
    -DEXECUTORCH_SELECT_OPS_LIST="aten::_softmax.out"  \
    -B$(pwd)/cmake-out/examples/arm                   \
    $(pwd)/examples/arm

## How to set up this project:
### OpenOCD
> sudo apt-get install openocd

### pyOCD
> source mbed-os/venv/bin/activate
> python3 -m pip install -U pyocd

> pyocd pack --update

> pyocd pack --install stm32wb55rg

### Connect with serial port
Open a new terminal. Plug in Nucleo-WB55RG. Install Mbed CLI 2 and use it to find serial port.
> sudo apt-get install python3-dev

> python3 -m pip install mbed-tools

> mbed-tools detect

Remember serial port. Use minicom to see console output. 
> minicom -D /dev/ttyACM0
ctrl a & x to exit

### Create your model_pte.h file
https://pytorch.org/executorch/main/executorch-arm-delegate-tutorial.html
Suitable script will be added soon but the following command is important:
> python3 -m examples.arm.aot_arm_compiler --model_name="softmax"
