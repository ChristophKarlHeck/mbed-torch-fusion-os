# Mbed OS upload method configuration file for target DISCO_F469NI.
# To change any of these parameters from their default values, set them in your build script between where you
# include app.cmake and where you add mbed os as a subdirectory.

# Notes:
# 1. Using the JLINK upload method with your dev board requires converting its ST-LINK into a J-Link.  See here for details: https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/
# 2. The PYOCD upload method is disabled by default, because it works only firs time after full erase from STM32Cube and failed every time after.

# General config parameters
# -------------------------------------------------------------
set(UPLOAD_METHOD_DEFAULT MBED)

# Config options for MBED
# -------------------------------------------------------------

set(MBED_UPLOAD_ENABLED TRUE)
set(MBED_RESET_BAUDRATE 115200)

# Config options for JLINK
# -------------------------------------------------------------

set(JLINK_UPLOAD_ENABLED TRUE)
set(JLINK_CPU_NAME STM32F469NI)
set(JLINK_CLOCK_SPEED 4000)
set(JLINK_UPLOAD_INTERFACE SWD)

# Config options for PYOCD
# -------------------------------------------------------------

set(PYOCD_UPLOAD_ENABLED FALSE)
set(PYOCD_TARGET_NAME stm32f469nihx)
set(PYOCD_CLOCK_SPEED 4000k)

# Config options for OPENOCD
# -------------------------------------------------------------

set(OPENOCD_UPLOAD_ENABLED TRUE)
set(OPENOCD_CHIP_CONFIG_COMMANDS
    -f ${OpenOCD_SCRIPT_DIR}/board/stm32f4discovery.cfg)

# Config options for STM32Cube
# -------------------------------------------------------------

set(STM32CUBE_UPLOAD_ENABLED TRUE)
set(STM32CUBE_CONNECT_COMMAND -c port=SWD reset=HWrst)
set(STM32CUBE_GDBSERVER_ARGS --swd)

# Config options for stlink
# -------------------------------------------------------------

set(STLINK_UPLOAD_ENABLED TRUE)
set(STLINK_LOAD_ADDRESS 0x8000000)
set(STLINK_ARGS --connect-under-reset)
