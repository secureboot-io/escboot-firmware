## Setup cross-compile toolchain 


# Target-specific flags
set(MCU_ARCH ARMCM3)
set(MCU_FAMILY STM32F051x8)
set(MCU_NAME STM32F051x8)

# You can find the following values from ./cubemx/Makefile
set(CPU "-mcpu=cortex-m0")
set(FPU "")
set(FLOAT_ABI "")

set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/linker/target/stm32f051/STM32F051R8TX_FLASH.ld)

option(USE_LL_LIB "Enable LL library" ON)
option(USE_HAL_LIB "Enable HAL library" ON)

option(USE_SYSTEM_VIEW "Enable Segger SystemView library" OFF)

## auto-set variables from user input
set(MCU_FLAGS "${CPU} -mthumb ${FPU} ${FLOAT_ABI} -O2 -Os -std=c17 -ffile-prefix-map=${STM32_CMAKE_DIR}/= --specs=nano.specs")
set(LINK_FLAGS "-Wl,--gc-sections -Wl,--print-memory-usage -O2 -Os")
set(EXTRA_LINK_FLAGS "-Wl,-Map=${PROJECT_NAME}.map,--cref,--no-warn-mismatch --specs=nosys.specs")
include(${STM32_CMAKE_DIR}/cmake/stm32-gcc-flags.cmake)

# print summary of configurations
message(STATUS "MCU_FAMILY: ${MCU_FAMILY}")
message(STATUS "MCU_NAME: ${MCU_NAME}")
message(STATUS "Specified C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Linker script: ${LINKER_SCRIPT}")
message(STATUS "Use LL library: ${USE_LL_LIB}")
message(STATUS "Use HAL library: ${USE_HAL_LIB}")
message(STATUS "Use Segger SystemView library: ${USE_SYSTEM_VIEW}")

## version information
set(VERSION_MAJOR 0 CACHE STRING "Project major version number.")
set(VERSION_MINOR 1 CACHE STRING "Project minor version number.")
set(VERSION_PATCH 0 CACHE STRING "Project patch version number.")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/target/stm32f051)
if(USE_SYSTEM_VIEW)
add_definitions("-DENABLE_SYSTEMVIEW")
endif()

## add project components
set(ELF_TARGET ${PROJECT_NAME}.elf)
set(PROJ_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../libraries)

# project-specific modules
# add_subdirectory(driver)

# common libraries
add_subdirectory(${PROJ_LIB_PATH}/CMSIS ${CMAKE_BINARY_DIR}/libraries/CMSIS)
add_subdirectory(${PROJ_LIB_PATH}/STM32F0xx_HAL_Driver ${CMAKE_BINARY_DIR}/libraries/STM32F0xx_HAL_Driver)
add_subdirectory(${PROJ_LIB_PATH} ${CMAKE_BINARY_DIR}/libraries)

set(TARGET_STM32F407VGTX_SRC
    src/target/stm32f407vgtx/main.c
    src/target/stm32f407vgtx/stm32f4xx_it.c
    src/target/stm32f407vgtx/stm32f4xx_hal_msp.c
    src/target/stm32f407vgtx/system_stm32f4xx.c
    src/target/stm32f407vgtx/syscalls.c
    src/target/stm32f407vgtx/sysmem.c
    src/target/stm32f407vgtx/flash.c
    src/target/stm32f407vgtx/utils/micros.c
    src/target/stm32f407vgtx/utils/millis.c
    src/target/stm32f407vgtx/io/gpio.c
    src/target/stm32f407vgtx/io/uart.c
    src/target/stm32f407vgtx/startup/startup_stm32f407vgtx.s
)

set(TARGET_STM32F051_SRC
    src/target/stm32f051/main.c
    src/target/stm32f051/stm32f0xx_it.c
    src/target/stm32f051/system_stm32f0xx.c
    src/target/stm32f051/syscalls.c
    src/target/stm32f051/sysmem.c
    src/target/stm32f051/flash.c
    src/target/stm32f051/utils/micros.c
    src/target/stm32f051/utils/millis.c
    src/target/stm32f051/io/gpio.c
    src/target/stm32f051/io/uart.c
    src/target/stm32f051/crypto/cmox_low_level.c
    src/target/stm32f051/crypto/ecc.c
    src/target/stm32f051/crypto/entropy.c
    src/target/stm32f051/startup/startup_stm32f051r8tx.s
)

# STM32CubeMX generated source files
set(CUBEMX_SRC
    ${TARGET_STM32F051_SRC}
    src/utils/hexdump.c
    src/io/serialwire.c
    src/debug/logging.c
    src/bootloader.c
    src/secureboot.c
)
add_executable(${ELF_TARGET} ${CUBEMX_SRC})
target_compile_definitions(${ELF_TARGET} PUBLIC -D${MCU_FAMILY})
target_link_libraries(${ELF_TARGET} PUBLIC cmox libcrc cmsis stm32f0xx_hal  -lm -lc )
target_link_libraries(${ELF_TARGET} PRIVATE -T${LINKER_SCRIPT} ${EXTRA_LINK_FLAGS})
target_include_directories(${ELF_TARGET} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/target/stm32f051>)

## add post-build targets
include(${STM32_CMAKE_DIR}/cmake/stm32-gcc-postbuild.cmake)
# include(${CMAKE_CURRENT_SOURCE_DIR}/${STM32_PROJ_FOLDER}/stm32-gcc-jlink.cmake)
