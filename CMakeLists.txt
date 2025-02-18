cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
set(PICO_BOARD pico CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(ADC C CXX ASM)
pico_sdk_init()

add_executable(ADC ADC.c inc/ssd1306.c)

pico_set_program_name(ADC "ADC")
pico_set_program_version(ADC "0.1")
pico_enable_stdio_uart(ADC 0)
pico_enable_stdio_usb(ADC 1)

target_link_libraries(ADC pico_stdlib hardware_adc hardware_pwm hardware_i2c hardware_pio)
target_include_directories(ADC PRIVATE ${CMAKE_CURRENT_LIST_DIR} inc)

pico_add_extra_outputs(ADC)
