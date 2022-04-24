cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(pico_ov2640 C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico)
set(OV2640_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

pico_sdk_init()

file(GLOB SRC_FILES ${OV2640_ROOT_PATH}/src/*.c)
add_executable(pico_ov2640 ${SRC_FILES})

pico_generate_pio_header(pico_ov2640 ${OV2640_ROOT_PATH}/src/image.pio)

target_link_libraries(pico_ov2640
	pico_multicore
	pico_stdlib
    pico_time
    pico_util
    hardware_sync
    hardware_uart
    hardware_i2c
    hardware_dma
    hardware_pwm
    hardware_irq
    tinyusb_device
    hardware_pio
)

target_include_directories(pico_ov2640 PUBLIC
    ${OV2640_ROOT_PATH}
    ${OV2640_ROOT_PATH}/src
)

pico_add_extra_outputs(pico_ov2640)
