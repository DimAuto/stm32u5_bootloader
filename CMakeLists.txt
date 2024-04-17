cmake_minimum_required(VERSION 3.12)

project(remote_keyboard)
set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/PROJECT)

set(MCU_FAMILY STM32U5xx)
set(MCU_MODEL STM32U575xx)
# set(CPU_PARAMETERS
#     -mcpu=cortex-m33
#     -mfpu=fpv4-sp-d16
#     -mfloat-abi=hard
#     -mthumb)
     
set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/Core/Startup/startup_stm32u575ritxq.s)
set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32U575RITXQ_FLASH.ld)
# ------------------------------------------------------------------------------
set(EXECUTABLE ${CMAKE_PROJECT_NAME})
enable_language(C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

# Headers
set(STM32U_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${MCU_FAMILY}/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include)
set(PROJECT_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR})
    #${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_DIR})



#Sources
file(GLOB_RECURSE STM32U_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/*.c) 
file(GLOB_RECURSE PROJECT_SOURCES FOLLOW_SYMLINKS
    ${PROJECT_DIR}/*.c)


add_executable(${EXECUTABLE}
            ${STM32U_SOURCES}
            ${PROJECT_SOURCES}
            ${STARTUP_SCRIPT})


 # MACROS defines !!           
target_compile_definitions(${EXECUTABLE} PRIVATE
                            ${MCU_MODEL} USE_HAL_DRIVER)

target_include_directories(${EXECUTABLE} PRIVATE
                            ${STM32U_INCLUDE_DIRECTORIES}
                            ${PROJECT_INCLUDE_DIRECTORIES})

# target_compile_options(${EXECUTABLE} PRIVATE
#                         ${CPU_PARAMETERS})

target_link_options(${EXECUTABLE} PRIVATE
                    -T${MCU_LINKER_SCRIPT}
                    -mcpu=cortex-m33
                    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
                    --specs=nosys.specs
                    -Wl,--start-group
                    -Wl,--gc-sections
                    -static
                    -mfpu=fpv5-sp-d16
                    -mfloat-abi=hard
                    -mthumb
                    -u _printf_float
                    -Wl,--start-group
                    -lc
                    -lm
                    -Wl,--end-group
                    )

# Calculate the size of executable
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
                    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>)

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
                    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}>
                    ${EXECUTABLE}.hex
                    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}>
                    ${EXECUTABLE}.bin)