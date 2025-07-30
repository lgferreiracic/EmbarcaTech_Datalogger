cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)
set(FREERTOS_KERNEL_PATH "C:/FreeRTOS-Kernel")
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(Datalogger C CXX ASM)
pico_sdk_init()
add_subdirectory(lib/FatFs_SPI)    
include_directories(${CMAKE_SOURCE_DIR}/lib)
add_executable(${PROJECT_NAME} 
        Datalogger.c
        hw_config.c 
        lib/button.c
        lib/buzzer.c
        lib/led_rgb.c
        lib/ssd1306.c
        lib/mpu6050.c
        lib/sdcard.c
        lib/sensors.c
        lib/my_tasks.c
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        PICO_PRINTF_SUPPORTS_FLOAT=1
        PICO_STDIO_ENABLE_PRINTF=1
)
target_link_libraries(${PROJECT_NAME} 
            pico_stdlib
            pico_time
            hardware_i2c
            hardware_adc
            hardware_pwm
            hardware_timer
            hardware_gpio
            hardware_pio
            hardware_clocks
            hardware_uart
            FatFs_SPI
            FreeRTOS-Kernel       
            FreeRTOS-Kernel-Heap4
)
target_include_directories(${PROJECT_NAME}  PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${PICO_SDK_PATH}/lib/lwip/src/include
)
pico_enable_stdio_uart(${PROJECT_NAME}  0)
pico_enable_stdio_usb(${PROJECT_NAME}  1)

pico_add_extra_outputs(${PROJECT_NAME})
