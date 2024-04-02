# Mbed Torch Fusion OS
MbedTorch Fusion OS: Seamlessly merges Mbed OS with Torch ML models for the P-Nucleo-WB55RG on Arm Cortex M4. Empowering edge computing with efficient ML deployment.

## 1. Build ExecuTorch libraries
> git clone https://github.com/ChristophKarlHeck/mbed-torch-fusion-os.git

> cd mbed-torch-fusion-os/utils/scripts

> ./build_et_libs.sh

> cd ../../


## 2. Install pyOCD in mbed-os venv
> source mbed-os/venv/bin/activate

> python3 -m pip install -U pyocd

> pyocd pack --update

> pyocd pack --install stm32wb55rg

> deactivate

## 3. Convert softmax.pte file to model_pte.h

> cd executorch

> source .executorch/bin/activate

> export PATH="$(pwd)/third-party/flatbuffers/cmake-out:${PATH}"

> "$(pwd)/build/install_flatc.sh"

> export PATH="$(pwd)/../gcc-arm-none-eabi-10.3-2021.10/bin:${PATH}"

> hash arm-none-eabi-gcc

> cd ..

> python3 executorch/examples/arm/executor_runner/pte_to_header.py --pte executorch/softmax.pte --outdir .

## 4. Connect with serial port
Open a new terminal. Plug in Nucleo-WB55RG. Install Mbed CLI 2 and use it to find serial port.
> sudo apt-get install python3-dev

> python3 -m pip install mbed-tools

> mbed-tools detect

Remember serial port. Use minicom to see console output.

> minicom -D /dev/ttyACM0

ctrl a & x to exit

### 5. Build and flash using VS Code
https://github.com/mbed-ce/mbed-os/wiki/Project-Setup:-VS-Code
