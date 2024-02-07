/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <stdbool.h>
#include "checksum.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define RestartBootloader   0
#define ExitBootloader      1

#define CMD_RUN             0x00
#define CMD_PROG_FLASH      0x01
#define CMD_ERASE_FLASH     0x02
#define CMD_READ_FLASH_SIL  0x03
#define CMD_VERIFY_FLASH    0x03
#define CMD_VERIFY_FLASH_ARM 0x04
#define CMD_READ_EEPROM     0x04
#define CMD_PROG_EEPROM     0x05
#define CMD_READ_SRAM       0x06
#define CMD_READ_FLASH_ATM  0x07
#define CMD_KEEP_ALIVE      0xFD
#define CMD_SET_ADDRESS     0xFF
#define CMD_SET_BUFFER      0xFE

#define CMD_BOOTINIT        0x07
#define CMD_BOOTSIGN        0x08

#define START_BIT_TIMEOUT_MS 2

#define BIT_TIME (52)       // 52uS
#define BIT_TIME_HALVE      (BIT_TIME >> 1) // 26uS
#define BIT_TIME_3_4        (BIT_TIME_HALVE + (BIT_TIME_HALVE >> 1))   // 39uS
#define START_BIT_TIME      (BIT_TIME_3_4)

uint32_t millis() {
	return HAL_GetTick();
}

uint32_t micros(void)
{
  /* Ensure COUNTFLAG is reset by reading SysTick control and status register */
  LL_SYSTICK_IsActiveCounterFlag();
  uint32_t m = HAL_GetTick();
  const uint32_t tms = SysTick->LOAD + 1;
  __IO uint32_t u = tms - SysTick->VAL;
  if (LL_SYSTICK_IsActiveCounterFlag()) {
    m = HAL_GetTick();
    u = tms - SysTick->VAL;
  }
  return (m * 1000 + (u * 1000) / tms);
}

static void pinSetInputPullDown(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin       = GPIO_PIN_1;
    GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void pinSetOutput(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin       = GPIO_PIN_1;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    //GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void pinInit(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
    pinSetInputPullDown();
}

uint8_t deviceInfo[9] = {0x34,0x37,0x31,0x00,0x1f,0x06,0x06,0x01,0x30};

static bool readChar(uint8_t *buffer)
{
    uint32_t bitTime = millis() + START_BIT_TIMEOUT_MS;

    // Wait for RX to go low
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1)) {
    	if(millis() > bitTime) {
    		return false;
    	}
    }

    // Go to 1/4 of start bit
    bitTime = micros() + (BIT_TIME_HALVE>>2);
    while (micros() < bitTime);

    uint16_t bitmask = 0;
    uint8_t bit = 0;
    while (1) {
    	//Read bit
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1))
        {
            bitmask |= (1 << bit);
        }

        // Increment bit and return if 10 bits are read
        // don't wait for stop bit
        bit++;
        if(bit == 10)
        	break;

        // go to 1/4 of next bit
        bitTime = bitTime + BIT_TIME;
        while (micros() < bitTime);
    }

    // check start bit and stop bit
    if ((bitmask & 1) || (!(bitmask & (1 << 9)))) {
        return false;
    }

    // Set char to buffer
    *buffer = bitmask >> 1;

    // Success
    return true;
}

static bool writeChar(uint8_t buffer)
{

	uint16_t bitmask = buffer;

	// Add start/stop bits
	bitmask <<= 1;
	bitmask |= 1 << 9;

	// Start time
	uint32_t bitTime = micros();
	for(int i = 0; i < 10; i++) {
		//Send bit
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, bitmask & 1 ? GPIO_PIN_SET : GPIO_PIN_RESET);

		//Delay
		bitTime += BIT_TIME;
		while (micros() < bitTime);

		//Shift
		bitmask >>= 1;
	}

    // Success
    return true;
}

static bool readBuffer(uint8_t *buffer, size_t bufferLen, size_t *outLen) {
	int i = 0;

	//
	pinSetInputPullDown();

	// Read specified number of bytes
	while(i < bufferLen) {
		if(readChar(buffer + i)) {
			i++;
		} else {
			// return if error reading
			break;
		}
	}

	// Set output length
	*outLen = i;

	// Success if atleast 1 byte read
	return i != 0;
}

static bool writeBuffer(uint8_t *buffer, size_t bufferLen) {
	bool success;

	//
	pinSetOutput();

	while(bufferLen--)
	{
		success = writeChar(*buffer++);
		if(!success) {
			return false;
		}
	}

	return success;
}

//0000000000000000000000000D424C48656C69F47D
//static uint8_t BL_ReadBuf(uint8_t *pstring, uint8_t len)
//{
//    // len 0 means 256
//    //CRC_16.word = 0;
//    //LastCRC_16.word = 0;
//    //uint8_t  LastACK = brNONE;
//    do {
//        if (!suart_getc_(pstring)) goto timeout;
//        ByteCrc(pstring);
//        pstring++;
//        len--;
//    } while (len > 0);
//
////    if (isMcuConnected()) {
////        //With CRC read 3 more
////        if (!suart_getc_(&LastCRC_16.bytes[0])) goto timeout;
////        if (!suart_getc_(&LastCRC_16.bytes[1])) goto timeout;
////        if (!suart_getc_(&LastACK)) goto timeout;
////        if (CRC_16.word != LastCRC_16.word) {
////            LastACK = brERRORCRC;
////        }
////    } else {
//        if (!suart_getc_(&LastACK)) goto timeout;
////    }
//timeout:
//    return (LastACK == brSUCCESS);
//}

/* USER CODE END 0 */

//uint8_16_u makeCrc(uint8_t* pBuff, uint16_t length)
//{
//	uint8_16_u crc16;
//
//	crc16.word=0;
//
//	for(int i = 0; i < length; i++) {
//		 uint8_t xb = pBuff[i];
//		 for (uint8_t j = 0; j < 8; j++)
//		 {
//			 if (((xb & 0x01) ^ (crc16.word & 0x0001)) !=0 ) {
//				 crc16.word = CRC_16.word >> 1;
//				 crc16.word = CRC_16.word ^ 0xA001;
//			 } else {
//				 crc16.word = CRC_16.word >> 1;
//			 }
//			 xb = xb >> 1;
//		 }
//	 }
//	return crc16;
//
//}

typedef union {
	uint16_t word;
	uint8_t bytes[2];
} uint8_16_u;

bool checkCrc(uint8_t* pBuff, uint16_t length)
{
	uint8_16_u receivedCrc16;

	uint16_t calculatedCrc16 = crc_16(pBuff, length - 2);

	receivedCrc16.bytes[0] = pBuff[length - 2];
	receivedCrc16.bytes[1] = pBuff[length - 1];
	return receivedCrc16.word == calculatedCrc16;
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  pinInit();
  char chs[] = "0123456789ABCDEF";
  while (1)
    {
	  //HAL_UART_
        //printf("Hello World!!\n");
        //HAL_Delay(500);
	  uint8_t buf[248];
	  size_t outLen;
	  if(readBuffer(buf, 248, &outLen))
	  {
	  //uint_t *b = ch;
	  //int i = 0;
	  //while(1) {while(!suart_getc_(ch+i)){};i++; if(i==10) break;};
		  char st[248*2+1];
		  for(int i=0;i<outLen;i++) {st[i*2+1] = chs[buf[i] & 0xf]; st[i*2] = chs[buf[i] >> 4];};
		  st[outLen*2] = '\r';
		  st[outLen*2+1] = '\n';
		  HAL_UART_Transmit(&huart1, (uint8_t*)st, outLen*2+2, HAL_MAX_DELAY);
		  if(outLen == 21 && buf[20] == 0x7d) {
			  writeBuffer(deviceInfo, 9);
		  } else {
			  if(outLen >= 3) {
				  if(checkCrc(buf, outLen)) {
					  HAL_UART_Transmit(&huart1, (uint8_t*)"A\r\n", 3, HAL_MAX_DELAY);
				  } else {
					  HAL_UART_Transmit(&huart1, (uint8_t*)"N\r\n", 3, HAL_MAX_DELAY);
				  }
			  }
		  }
	  }
    }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
