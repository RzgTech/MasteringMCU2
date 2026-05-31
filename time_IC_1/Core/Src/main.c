/*
 * main.c
 *
 *  Created on: May 8, 2026
 *      Author: Vahid
 */

#include "main.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

void SystemCLockConfig(uint8_t clocl_freq);
void GPIO_Init(void);
void Timer2_Init(void);
void LSE_Config(void);
void UART2_Init(void);
void Error_handler(void);

UART_HandleTypeDef huart;  //UART2 handle
TIM_HandleTypeDef htimer2;  //timer2 is general purpose timer

uint32_t capture_values[2] = {0};  //to store the captured values of the input signal 
uint8_t is_capture_done = FALSE;  //flag to indicate that the capture is done and we can calculate the frequency
uint8_t counter = 1;

char msg[100];  //to store the message to be sent over UART

int main()
{
	uint32_t diff = 0;  //to store the difference between the two captures (in timer counts)
	double timer2_cnt_freq = 0;
	double timer2_cnt_resolution = 0;
	double user_signal_time_period = 0;
	double user_signal_frequency = 0;

	HAL_Init();
	SystemCLockConfig(SYS_CLOCK_FREQ_50_MHZ);
	GPIO_Init();

	UART2_Init();
	Timer2_Init();

	LSE_Config();

	HAL_TIM_IC_Start_IT(&htimer2, TIM_CHANNEL_1);

	while(1)
	{
		if (is_capture_done == TRUE)
		{

			if (capture_values[1] > capture_values[0])
			{
				diff = capture_values[1] - capture_values[0];
			}
			else if (capture_values[1] < capture_values[0])  //this is the case when the counter overflows between the two captures
			{
				diff = (0xFFFFFFFF - capture_values[0]) + capture_values[1];
			}


			timer2_cnt_freq = (HAL_RCC_GetPCLK1Freq()*2) / (htimer2.Init.Prescaler + 1);  //calculating the timer counter clock frequency (in Hz)
			timer2_cnt_resolution = 1 / timer2_cnt_freq;  //calculating the timer tick time (in seconds)
			user_signal_time_period = diff * timer2_cnt_resolution;

			user_signal_frequency = 1 / user_signal_time_period;  //calculating the frequency (in Hz)

			sprintf(msg, "Frequency: %f Hz\r\n", user_signal_frequency);
			HAL_UART_Transmit(&huart, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

			is_capture_done = FALSE;  //resetting the flag for the next captures
		}
	}
	return 0;
}

void SystemCLockConfig(uint8_t clocl_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	uint32_t FLatency = 0;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_HSE;
	osc_init.HSIState = RCC_HSI_ON; //by default it is ON, but anyway, we write it
	osc_init.LSEState = RCC_LSE_ON;
	osc_init.HSEState = RCC_HSE_BYPASS;
	osc_init.HSICalibrationValue = 16;  //by default it is 16, but anyway, we write it
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	osc_init.PLL.PLLState = RCC_PLL_ON;

	switch (clocl_freq)
	{
	case SYS_CLOCK_FREQ_50_MHZ:
	{
		osc_init.PLL.PLLM = 16;
		osc_init.PLL.PLLN = 100;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLQ = 2; //default value
		osc_init.PLL.PLLR = 2; //default value

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_1WS;
		break;
	}
	case SYS_CLOCK_FREQ_84_MHZ:
	{
		osc_init.PLL.PLLM = 16;
		osc_init.PLL.PLLN = 168;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLQ = 2; //default value
		osc_init.PLL.PLLR = 2; //default value

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
		osc_init.PLL.PLLM = 16;
		osc_init.PLL.PLLN = 240;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLQ = 2; //default value
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

	if (HAL_RCC_ClockConfig(&clk_init, FLatency))
	{
		Error_handler();
	}

	//Systick configuration
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);


}

void GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();  //enabling GPIOA clock

	GPIO_InitTypeDef ledgpio;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pin = GPIO_PIN_5;
	ledgpio.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOA, &ledgpio);

}

void Timer2_Init(void)
{
	TIM_IC_InitTypeDef timer2IC_Config;

	//initializing time base

	htimer2.Instance = TIM2;
	htimer2.Init.Prescaler = 1;
	htimer2.Init.Period = 0xFFFFFFFF;	//RM >> 17.4 -> 32-bit counter
	htimer2.Init.CounterMode = TIM_COUNTERMODE_UP;  //this is the default

	if (HAL_TIM_IC_Init(&htimer2)!= HAL_OK)
	{
		Error_handler();
	}

	//configuring input channel

	timer2IC_Config.ICFilter = 0;
	timer2IC_Config.ICPolarity = TIM_ICPOLARITY_RISING;
	timer2IC_Config.ICPrescaler = TIM_ICPSC_DIV1;
	timer2IC_Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
	if (HAL_TIM_IC_ConfigChannel(&htimer2, &timer2IC_Config, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_handler();
	}


}

void LSE_Config(void)
{
#if 0
	RCC_OscInitTypeDef osc_init;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_LSE;
	osc_init.LSEState = RCC_LSE_ON;

	if (HAL_RCC_OscConfig(&osc_init)!= HAL_OK)
	{
		Error_handler();
	}
#endif

	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_4);  //this method configures lse as output on mco1
																    //inside the method the gpio configurations are done (e.g. PA8 as MCO1)
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (is_capture_done == FALSE)
	{
		if (counter == 1)
		{
			capture_values[0] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
			counter++;
		}
		else if (counter == 2)
		{
			capture_values[1] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
			counter = 1;
			is_capture_done = TRUE;
		}
	}

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
