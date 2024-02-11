set(STM32F0_HAL_SRC
    #  Src/stm32f0xx_hal.c
    # # Src/stm32f0xx_hal_adc.c
    # # Src/stm32f0xx_hal_adc_ex.c
    # # Src/stm32f0xx_hal_can.c
    # # Src/stm32f0xx_hal_cec.c
    # Src/stm32f0xx_hal_cortex.c
    # # Src/stm32f0xx_hal_crc.c
    # # Src/stm32f0xx_hal_dac.c
    # # Src/stm32f0xx_hal_dac_ex.c
    # Src/stm32f0xx_hal_dma.c
    # # Src/stm32f0xx_hal_eth.c
    # Src/stm32f0xx_hal_exti.c
    # Src/stm32f0xx_hal_flash.c
    # Src/stm32f0xx_hal_flash_ex.c
    # Src/stm32f0xx_hal_gpio.c
    # # Src/stm32f0xx_hal_gpio_ex.c
    # # Src/stm32f0xx_hal_hcd.c
    # Src/stm32f0xx_hal_i2c.c
    # # Src/stm32f0xx_hal_i2s.c
    # # Src/stm32f0xx_hal_irda.c
    # # Src/stm32f0xx_hal_iwdg.c
    # # Src/stm32f0xx_hal_mmc.c
    # # Src/stm32f0xx_hal_msp_template.c
    # # Src/stm32f0xx_hal_nand.c
    # # Src/stm32f0xx_hal_nor.c
    # # Src/stm32f0xx_hal_pccard.c
    # # Src/stm32f0xx_hal_pcd.c
    # # Src/stm32f0xx_hal_pcd_ex.c
    # Src/stm32f0xx_hal_pwr.c
    # Src/stm32f0xx_hal_rcc.c
    # Src/stm32f0xx_hal_rcc_ex.c
    # # Src/stm32f0xx_hal_rtc.c
    # # Src/stm32f0xx_hal_rtc_ex.c
    # # Src/stm32f0xx_hal_sd.c
    # # Src/stm32f0xx_hal_smartcard.c
    # # Src/stm32f0xx_hal_spi.c
    # # Src/stm32f0xx_hal_sram.c
    # Src/stm32f0xx_hal_tim.c
    # # Src/stm32f0xx_hal_timebase_rtc_alarm_template.c
    # # Src/stm32f0xx_hal_timebase_tim_template.c
    Src/stm32f0xx_ll_dma.c
    Src/stm32f0xx_ll_exti.c
    Src/stm32f0xx_ll_gpio.c
    Src/stm32f0xx_ll_pwr.c
    Src/stm32f0xx_ll_rcc.c
    Src/stm32f0xx_ll_usart.c
    Src/stm32f0xx_ll_utils.c
)

set(STM32F0_LIB_SRC ${STM32F0_HAL_SRC})

add_library(stm32f0xx_hal ${STM32F0_LIB_SRC})
target_compile_definitions(stm32f0xx_hal PUBLIC -DUSE_FULL_LL_DRIVER)
target_link_libraries(stm32f0xx_hal PUBLIC cmsis)
target_include_directories(stm32f0xx_hal PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Inc>
  PRIVATE Src)
