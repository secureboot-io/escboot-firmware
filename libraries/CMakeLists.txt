set(LIBCRC_SRC
    libcrc/src/crc16.c
)

add_library(libcrc ${LIBCRC_SRC})
target_include_directories(libcrc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libcrc/include)
