/*
 * main.c
 *
 *  Created on: Jun 8, 2026
 *      Author: Vahid
 */

#include "main_app.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

void SystemCLock_Config_HSE(uint8_t clocl_freq);
void GPIO_Init(void);
void UART2_Init();
void Error_handler(void);
void GPIO_AnalogConfig(void);

UART_HandleTypeDef huart;  //UART2 handle
extern uint8_t some_data[];

/**
  * @brief  Print a string to console over UART.
  * @param  format: Format string as in printf.
  * @param  ...: Additional arguments providing the data to print.
  * @retval None
  */
void printmsg(char *format,...)
{
  char str[80];

  /*Extract the the argument list using VA apis */
  va_list args;
  va_start(args, format);
  vsprintf(str, format,args);
  HAL_UART_Transmit(&huart,(uint8_t *)str, strlen(str),HAL_MAX_DELAY);
  va_end(args);
}

int main()
{
	uint32_t *pBackupSRAM = 0;
	char data[] = "Hello";

	HAL_Init();

	GPIO_Init();

	SystemCLock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);
	UART2_Init();

	//1. enabling clock for Backup SRAM
	__HAL_RCC_BKPSRAM_CLK_ENABLE();

	//2. enabling write access to backup SRAM
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();

	pBackupSRAM = (uint32_t*)BKPSRAM_BASE;

	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) //to understand if the reset was due to the standby or not
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB); //set by the hardware and should be cleared by software
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
		printmsg("woke up from the standby mode\r\n");
		//reading back the backup SRAM after waking up from standby mode
		uint8_t data = (uint8_t)*pBackupSRAM; //comparing only the first byte
		if (data != 'H')
		{
			printmsg("BACKUP SRAM data lost\r\n");
		}
		else
		{
			printmsg("BACKUP SRAM data safe\r\n");
		}
	}
	else
	{
		for(uint32_t i=0; i<strlen(data)+1; i++)
		{
			*(pBackupSRAM + i) = data[i];
		}
	}

	/*here we reset (system reset) to verify that the content inside the backup sram remains unchanged*/
	printmsg("Press the user button to enter standby mode\r\n");
	while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) != GPIO_PIN_RESET);

	//when user pushes the button it comes here
	printmsg("Going to Standby mode\r\n");

	//Enable the wakeup pin 1 in pwr_csr register
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	HAL_PWREx_EnableBkUpReg(); //enabling the backup voltage regulator to preserve the content of backup SRAM after waking up from the Standby mode

	HAL_PWR_EnterSTANDBYMode(); //when it wakes up using the wakeup pin from standby mode,
								//it undergoes system reset (it will not resume the codes after HAL_PWR_EnterSTANDBYMode();,
								//it starts form the beginning of the main.
								//we can realize the cause of the reset using an reset cause flag



	while(1);

	return 0;
}

void SystemCLock_Config_HSE(uint8_t clocl_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	uint32_t FLatency = 0;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState = RCC_HSE_BYPASS;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	osc_init.PLL.PLLState = RCC_PLL_ON;

	switch (clocl_freq)
	{
	case SYS_CLOCK_FREQ_50_MHZ:
	{
		osc_init.PLL.PLLM = 4;
		osc_init.PLL.PLLN = 50;
		osc_init.PLL.PLLP = RCC_PLLP_DIV2;
		osc_init.PLL.PLLQ = 2; //default value
		osc_init.PLL.PLLR = 2; //default value

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV1;

		FLatency = FLASH_ACR_LATENCY_1WS;
		break;
	}
	case SYS_CLOCK_FREQ_84_MHZ:
	{
		osc_init.PLL.PLLM = 4;
		osc_init.PLL.PLLN = 84;
		osc_init.PLL.PLLP = RCC_PLLP_DIV2;
		osc_init.PLL.PLLQ = 2; //default value
		osc_init.PLL.PLLR = 1; //default value

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_2WS;
		break;
	}
	case SYS_CLOCK_FREQ_120_MHZ:
	{
		osc_init.PLL.PLLM = 4;
		osc_init.PLL.PLLN = 120;
		osc_init.PLL.PLLP = RCC_PLLP_DIV2;
		osc_init.PLL.PLLQ = 4; //default value
		osc_init.PLL.PLLR = 2; //default value


		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_3WS;
		break;
	}
	default:
		return;
	}

	if (HAL_RCC_OscConfig(&osc_init)!= HAL_OK)
	{
		Error_handler();
	}

	if (HAL_RCC_ClockConfig(&clk_init, FLatency) != HAL_OK)
	{
		Error_handler();
	}

	//Systick configuration
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void GPIO_Init(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef buttongpio;

    buttongpio.Pin = GPIO_PIN_13;
    buttongpio.Mode = GPIO_MODE_INPUT;
    buttongpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC,&buttongpio);

}

/**
  * @brief  Configures specified GPIO pins of GPIOA and GPIOC ports as analog.
  * @retval None
  */
void GPIO_AnalogConfig(void)
{
  GPIO_InitTypeDef GpioA,GpioC;

  //skip GPIO 13 and 14 as they are SWDIO and SWD_CLK
  uint32_t gpio_pins = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_4  | \
                       GPIO_PIN_5  | GPIO_PIN_6  | GPIO_PIN_7  | \
                       GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | \
                       GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15;

  GpioA.Pin = gpio_pins;
  GpioA.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA,&GpioA);

  gpio_pins = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | \
              GPIO_PIN_3  | GPIO_PIN_4  | GPIO_PIN_5  | \
              GPIO_PIN_6  | GPIO_PIN_7  | GPIO_PIN_8  | \
              GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | \
              GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;

  GpioC.Pin = gpio_pins;
  GpioC.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOC,&GpioC);
}

void UART2_Init(void)
{
	huart.Instance = USART2;
	huart.Init.BaudRate = 115200;
	huart.Init.WordLength = UART_WORDLENGTH_8B;
	huart.Init.StopBits = UART_STOPBITS_1;
	huart.Init.Parity = UART_PARITY_NONE;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.Mode = UART_MODE_TX_RX;

	if (HAL_UART_Init(&huart) != HAL_OK)
	{
		//there is a problem
		Error_handler();
	}
}

void Error_handler(void)
{
	while(1);
}
