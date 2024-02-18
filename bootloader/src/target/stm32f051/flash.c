#include "flash.h"

#include "stm32f0xx_ll_crs.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_pwr.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_gpio.h"

#include <stdint.h>
#include <string.h>

#define page_size 0x400                   // 1 kb for f051
uint32_t FLASH_FKEY1 =0x45670123;
uint32_t FLASH_FKEY2 =0xCDEF89AB;

void flash_erase_page(uint32_t add)
{
	// unlock flash
	while ((FLASH->SR & FLASH_SR_BSY) != 0) {
	/*  add time-out*/
	}
	if ((FLASH->CR & FLASH_CR_LOCK) != 0) {
	FLASH->KEYR = FLASH_FKEY1;
	FLASH->KEYR = FLASH_FKEY2;
	}

	// erase page if address even divisable by 1024
	 if((add % 1024) == 0){
	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR = add;
	FLASH->CR |= FLASH_CR_STRT;
	while ((FLASH->SR & FLASH_SR_BSY) != 0){
	/*  add time-out */
	}
	if ((FLASH->SR & FLASH_SR_EOP) != 0){
	FLASH->SR = FLASH_SR_EOP;
	}
	else{
	/* error */
	}
	FLASH->CR &= ~FLASH_CR_PER;
	}
	 SET_BIT(FLASH->CR, FLASH_CR_LOCK);
}

void flash_write(uint8_t *data, int length, uint32_t add)
{
	uint16_t data_to_FLASH[length / 2];
	memset(data_to_FLASH, 0, length / 2);
	for(int i = 0; i < length / 2 ; i ++ ){
		data_to_FLASH[i] =  data[i*2+1] << 8 | data[i*2];   // make 16 bit
	}
	volatile uint32_t data_length = length / 2;

	// unlock flash

	while ((FLASH->SR & FLASH_SR_BSY) != 0) {
	/*  add time-out*/
	}
	if ((FLASH->CR & FLASH_CR_LOCK) != 0) {
	FLASH->KEYR = FLASH_FKEY1;
	FLASH->KEYR = FLASH_FKEY2;
	}

	// erase page if address even divisable by 1024
	 if((add % 1024) == 0){


	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR = add;
	FLASH->CR |= FLASH_CR_STRT;
	while ((FLASH->SR & FLASH_SR_BSY) != 0){
	/*  add time-out */
	}
	if ((FLASH->SR & FLASH_SR_EOP) != 0){
	FLASH->SR = FLASH_SR_EOP;
	}
	else{
	/* error */
	}
	FLASH->CR &= ~FLASH_CR_PER;

	 }

	 volatile uint32_t write_cnt=0, index=0;
	 while(index < data_length)
			  {

	    	FLASH->CR |= FLASH_CR_PG; /* (1) */
	    	*(__IO uint16_t*)(add+write_cnt) = data_to_FLASH[index];
	    	while ((FLASH->SR & FLASH_SR_BSY) != 0){ /*  add time-out  */
	    	}
	   	 if ((FLASH->SR & FLASH_SR_EOP) != 0){
	   	 FLASH->SR = FLASH_SR_EOP;
	   	 }
	   	 else{
	   	 /*  error  */
	   	 }
	   	 FLASH->CR &= ~FLASH_CR_PG;
				  write_cnt += 2;
				  index++;
		  }
	 SET_BIT(FLASH->CR, FLASH_CR_LOCK);
}
