# the name of the target operating system
set(CMAKE_SYSTEM_NAME   Generic)
set(CMAKE_SYSTEM_VERSION  1)
set(CMAKE_SYSTEM_PROCESSOR  Arm)

set(TOOLCHAIN_DIR ${PROJECT_BINARY_DIR}/external/toolchain)

set(CROSS_COMPILE_PREFIX ${TOOLCHAIN_DIR}/bin/arm-none-eabi-)

if(NOT EXISTS ${TOOLCHAIN_DIR})
    file(DOWNLOAD https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz ${TOOLCHAIN_DIR}/toolchain.tar.xz SHOW_PROGRESS)
    execute_process(COMMAND tar --strip-components=1 -xvf ${TOOLCHAIN_DIR}/toolchain.tar.xz WORKING_DIRECTORY ${TOOLCHAIN_DIR})
    execute_process(COMMAND rm ${TOOLCHAIN_DIR}/toolchain.tar.xz)
endif()

# specify the cross compiler
set(CMAKE_C_COMPILER ${CROSS_COMPILE_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE_PREFIX}gcc)
set(CMAKE_OBJCOPY ${CROSS_COMPILE_PREFIX}objcopy)
set(CMAKE_OBJDUMP ${CROSS_COMPILE_PREFIX}objdump)

# find additional toolchain executables
find_program(ARM_SIZE_EXECUTABLE ${CROSS_COMPILE_PREFIX}size)
find_program(ARM_GDB_EXECUTABLE ${CROSS_COMPILE_PREFIX}gdb)
find_program(ARM_OBJCOPY_EXECUTABLE ${CROSS_COMPILE_PREFIX}objcopy)
find_program(ARM_OBJDUMP_EXECUTABLE ${CROSS_COMPILE_PREFIX}objdump)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# search for program/library/include in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
