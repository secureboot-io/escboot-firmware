add_library(cmsis_at32 INTERFACE IMPORTED GLOBAL)
target_include_directories(cmsis_at32 INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cmsis/cm4/core_support>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cmsis/cm4/device_support>)

set(AT32_DRIVERS_SRC
  drivers/src/at32f421_crm.c
  drivers/src/at32f421_usart.c
  drivers/src/at32f421_gpio.c
  drivers/src/at32f421_misc.c
  drivers/src/at32f421_flash.c
  drivers/src/at32f421_tmr.c
  drivers/src/at32f421_ertc.c
  drivers/src/at32f421_pwc.c
)

add_library(libat32f421 ${AT32_DRIVERS_SRC})
target_compile_definitions(libat32f421 PUBLIC -DAT32F421C6T7)
target_link_libraries(libat32f421 cmsis_at32)
target_include_directories(libat32f421 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cmsis/cm4/core_support>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cmsis/cm4/device_support>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/drivers/inc>)