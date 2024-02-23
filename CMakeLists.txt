cmake_minimum_required(VERSION 3.9)
project(am32secureboot C CXX ASM)

# adjust project folder name if necessary
# set(STM32_PROJ_FOLDER stm32proj)

message(STATUS "Building project: ${PROJECT_NAME}")
set(STM32_CMAKE_DIR "${CMAKE_SOURCE_DIR}")

# set the default build type
if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  message(STATUS "Build type: Debug")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  message(STATUS "Build type: Release")
endif()

# include(CheckIPOSupported)
# check_ipo_supported(RESULT supported OUTPUT error)
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(CROSS_COMPILE_PREFIX arm-none-eabi-)
include(${STM32_CMAKE_DIR}/cmake/gcc-arm-none-eabi.cmake)

# add source folder
add_subdirectory(bootloader)

