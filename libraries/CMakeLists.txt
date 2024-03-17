set(LIBCRC_SRC
    libcrc/src/crc16.c
)

add_library(libcrc ${LIBCRC_SRC})
target_compile_options(libcrc PUBLIC "-mcpu=cortex-m0" "-mthumb")
target_include_directories(libcrc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libcrc/include)

add_library(micro-ecc micro-ecc/uECC.c)
target_include_directories(micro-ecc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/micro-ecc)

add_library(SHA256_embedded SHA256_embedded/sha256.c)
target_include_directories(SHA256_embedded PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/SHA256_embedded)

add_subdirectory(at32)
add_subdirectory(stm32)