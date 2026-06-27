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
void RTC_Init(void);
void RTC_CalendarConfig(void);
char* get_dayofweek(uint8_t number);

UART_HandleTypeDef huart;  //UART2 handle
RTC_HandleTypeDef hrtc;  //RTC handler

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
	HAL_Init();

	GPIO_Init();

	SystemCLock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);
	UART2_Init();

	RTC_Init();

	RTC_CalendarConfig();


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
    buttongpio.Mode = GPIO_MODE_IT_FALLING;
    buttongpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC,&buttongpio);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

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

void RTC_Init(void)
{
	hrtc.Instance = RTC;
	hrtc.Init.AsynchPrediv = 0x7F;
	hrtc.Init.SynchPrediv = 0xFF;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_LOW; //does not matter since output is disabled
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN; //does not matter since output is disabled
	hrtc.Init.HourFormat = RTC_HOURFORMAT_12;

	if (HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		Error_handler();
	}
}

void RTC_CalendarConfig(void)
{
	//this function does RTC calendar config
	//let's configure the calendar for time: 12:11:10 PM Date: 27 June 2026 Saturday

	RTC_TimeTypeDef rtc_timeInit;
	RTC_DateTypeDef rtc_dateInit;

	rtc_timeInit.Hours = 12;
	rtc_timeInit.Minutes = 11;
	rtc_timeInit.Seconds = 10;
	rtc_timeInit.TimeFormat = RTC_HOURFORMAT12_PM;

	if (HAL_RTC_SetTime(&hrtc, &rtc_timeInit, RTC_FORMAT_BIN)!=HAL_OK)
	{
		Error_handler();
	}

	rtc_dateInit.Date = 27;
	rtc_dateInit.Month = RTC_MONTH_JUNE;
	rtc_dateInit.Year = 26;
	rtc_dateInit.WeekDay = RTC_WEEKDAY_SATURDAY;

	if (HAL_RTC_SetDate(&hrtc, &rtc_dateInit, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_handler();
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	RTC_TimeTypeDef current_time;
	RTC_DateTypeDef current_date;
	char *cur_time_format;

	if (HAL_RTC_GetTime(&hrtc, &current_time, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_handler();
	}

	cur_time_format = current_time.TimeFormat == 0 ? "AM": "PM";

	if (HAL_RTC_GetDate(&hrtc, &current_date, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_handler();
	}

	printmsg("TIME: %02d:%02d:%02d %s\r\n", current_time.Hours, current_time.Minutes, current_time.Seconds, cur_time_format);
	printmsg("DATE: %02d/%2d/%2d <%s>\r\n", current_date.Date, current_date.Month, 2000 + current_date.Year, get_dayofweek(current_date.WeekDay));


}

char* get_dayofweek(uint8_t number)
{
	char* weekdays[] = {"MONDAY", "TUESDAY", "WEDNESADY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY"};

	return weekdays[number-1];
}

void Error_handler(void)
{
	while(1);
}
