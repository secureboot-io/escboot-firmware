add_library(cmox STATIC IMPORTED GLOBAL)
set_target_properties(cmox PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/STM32_Cryptographic/lib/libSTM32Cryptographic_CM0_CM0PLUS.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/STM32_Cryptographic/include
)

add_library(cmsis INTERFACE)
target_compile_definitions(cmsis INTERFACE -DSTM32F051x8)
target_include_directories(cmsis INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Core/Include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Device/ST/STM32F0xx/Include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Device/ST/STM32F1xx/Include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Device/ST/STM32F4xx/Include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Device/ST/STM32F7xx/Include>)

add_library(stm32f0xx_hal STATIC 
    STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_utils.c
    STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rcc.c)
target_compile_options(stm32f0xx_hal PUBLIC "-mcpu=cortex-m0" "-mthumb" "-O2" "-Os" "-std=c17" "-ffile-prefix-map=${STM32_CMAKE_DIR}/=" "-DENABLE_LOGGING")
target_compile_definitions(stm32f0xx_hal PUBLIC -DSTM32F0 -DUSE_FULL_LL_DRIVER)
target_link_libraries(stm32f0xx_hal PUBLIC cmsis)
target_include_directories(stm32f0xx_hal PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/STM32F0xx_HAL_Driver/Inc>)

add_library(stm32f4xx_hal STATIC 
    STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_utils.c
    STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rcc.c)
target_compile_definitions(stm32f4xx_hal PUBLIC -DSTM32F407xx -DUSE_FULL_LL_DRIVER)
target_link_libraries(stm32f4xx_hal PUBLIC cmsis)
target_include_directories(stm32f4xx_hal PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/STM32F4xx_HAL_Driver/Inc>)