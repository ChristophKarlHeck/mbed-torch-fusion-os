# Mbed Torch Fusion OS
MbedTorch Fusion OS: Seamlessly merges Mbed OS with Torch ML models for the P-Nucleo-WB55RG on Arm Cortex M4. Empowering edge computing with efficient ML deployment.

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
