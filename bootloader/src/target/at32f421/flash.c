#include "flash.h"

#include "at32f421.h"
#include "at32f421_flash.h"
// #include "stm32f0xx_ll_rcc.h"
// #include "stm32f0xx_ll_bus.h"
// #include "stm32f0xx_ll_system.h"
// #include "stm32f0xx_ll_exti.h"
// #include "stm32f0xx_ll_cortex.h"
// #include "stm32f0xx_ll_utils.h"
// #include "stm32f0xx_ll_pwr.h"
// #include "stm32f0xx_ll_dma.h"
// #include "stm32f0xx_ll_gpio.h"

#include <stdint.h>
#include <string.h>

#define page_size 0x400 // 1 kb for f051
uint32_t FLASH_FKEY1 = 0x45670123;
uint32_t FLASH_FKEY2 = 0xCDEF89AB;

void flErasePage(uint32_t add)
{

	// unlock flash

	flash_unlock();

	flash_sector_erase(add);

	flash_lock();
}

void flWrite(const uint8_t *data, int length, uint32_t add)
{
	uint32_t data_to_FLASH[length / 4];
	memset(data_to_FLASH, 0, length / 4);
	for (int i = 0; i < length / 4; i++)
	{
		data_to_FLASH[i] = data[i * 4 + 3] << 24 | data[i * 4 + 2] << 16 | data[i * 4 + 1] << 8 | data[i * 4]; // make 16 bit
	}
	volatile uint32_t data_length = length / 4;

	// erase page if address even divisable by 1024
	if ((add % 1024) == 0)
	{

		flErasePage(add);
	}

	// unlock flash
	flash_unlock();

	volatile uint32_t write_cnt = 0, index = 0;
	while (index < data_length)
	{

		//    	  fmc_word_program(add + (index*4),data_to_FLASH[index]);
		flash_word_program(add + (index * 4), data_to_FLASH[index]);
		//				fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
		flash_flag_clear(FLASH_PROGRAM_ERROR | FLASH_EPP_ERROR | FLASH_OPERATE_DONE);
		index++;
	}
	flash_lock();
}
