# Copyright (c) 2020 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

### stlink Upload Method
# See docs for valid parameters.

set(UPLOAD_SUPPORTS_DEBUG TRUE)

### Check if upload method can be enabled on this machine
find_package(stlink)
set(UPLOAD_STLINK_FOUND ${stlink_FOUND})

if(stlink_FOUND AND (stlink_VERSION VERSION_LESS 1.7.0))
	message(WARNING "Mbed OS may not work properly with stlink versions older than 1.7.0 (yours is ${stlink_VERSION}), as its command line options have changed.")
endif()

### Figure out --serial argument
if(DEFINED STLINK_PROBE_SN AND NOT "${STLINK_PROBE_SN}" STREQUAL "")
	set(STLINK_SERIAL_ARGUMENT --serial ${STLINK_PROBE_SN} CACHE INTERNAL "" FORCE)
else()
	set(STLINK_SERIAL_ARGUMENT "" CACHE INTERNAL "" FORCE)
endif()

### Function to generate upload target
function(gen_upload_target TARGET_NAME BIN_FILE)

	add_custom_target(flash-${TARGET_NAME}
		COMMENT "Flashing ${TARGET_NAME} with stlink..."
		COMMAND ${st-flash_PATH}
		--reset # Reset chip after flashing
		${STLINK_SERIAL_ARGUMENT}
		${STLINK_ARGS}
		write ${BIN_FILE} ${STLINK_LOAD_ADDRESS})

	add_dependencies(flash-${TARGET_NAME} ${TARGET_NAME})
endfunction(gen_upload_target)

### Commands to run the debug server.
set(UPLOAD_WANTS_EXTENDED_REMOTE TRUE)
set(UPLOAD_GDBSERVER_DEBUG_COMMAND
	${st-util_PATH}
	${STLINK_SERIAL_ARGUMENT}
	${STLINK_ARGS}
	--listen_port=${GDB_PORT}
	--multi)

# Reference: https://github.com/Marus/cortex-debug/blob/056c03f01e008828e6527c571ef5c9adaf64083f/src/stutil.ts#L39
# (except I had to change target-download to load for some reason)
set(UPLOAD_LAUNCH_COMMANDS
	"monitor halt"
	"monitor reset"
	"load"
	"tbreak main"
	"monitor reset"
)
set(UPLOAD_RESTART_COMMANDS
	"monitor reset"
)