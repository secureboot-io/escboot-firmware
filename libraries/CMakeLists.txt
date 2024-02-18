set(LIBCRC_SRC
    libcrc/src/crc16.c
)

add_library(libcrc ${LIBCRC_SRC})
target_include_directories(libcrc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libcrc/include)

add_library(cmox STATIC IMPORTED GLOBAL)
set_target_properties(cmox PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/STM32_Cryptographic/lib/libSTM32Cryptographic_CM0_CM0PLUS.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/STM32_Cryptographic/include
)