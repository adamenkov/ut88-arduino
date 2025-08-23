# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pimoroni_picolipo_16mb CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

set(PIMORONI_PICO_PATH "C:/github/pimoroni-pico")
include("${PIMORONI_PICO_PATH}/pimoroni_pico_import.cmake")
include("${PIMORONI_PICO_PATH}/libraries/gfx_pack/gfx_pack.cmake")
include("${PIMORONI_PICO_PATH}/common/pimoroni_bus.cmake")
include("${PIMORONI_PICO_PATH}/common/pimoroni_i2c.cmake")

project(pimoroni-gfx-pack-m5stack-kbcard C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(pimoroni-gfx-pack-m5stack-kbcard main.cpp)

pico_set_program_name(pimoroni-gfx-pack-m5stack-kbcard "pimoroni-gfx-pack-m5stack-kbcard")
pico_set_program_version(pimoroni-gfx-pack-m5stack-kbcard "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(pimoroni-gfx-pack-m5stack-kbcard 0)
pico_enable_stdio_usb(pimoroni-gfx-pack-m5stack-kbcard 0)

# Add the standard library to the build
target_link_libraries(pimoroni-gfx-pack-m5stack-kbcard pico_stdlib gfx_pack hardware_i2c pimoroni_i2c)

# Add the standard include files to the build
target_include_directories(pimoroni-gfx-pack-m5stack-kbcard PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${USERHOME}/.pico-sdk/sdk/2.2.0/src/rp2_common/hardware_pwm/include
        ${USERHOME}/.pico-sdk/sdk/2.2.0/src/rp2_common/hardware_spi/include
        ${PIMORONI_PICO_PATH}/common
)

pico_add_extra_outputs(pimoroni-gfx-pack-m5stack-kbcard)

