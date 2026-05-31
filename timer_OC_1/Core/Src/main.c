/*
 * main.c
 *
 *  Created on: May 31, 2026
 *      Author: Vahid
 */

#include "main.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

void SystemCLock_Config_HSE(uint8_t clocl_freq);
void GPIO_Init(void);
void Timer2_Init(void);

void UART2_Init(void);
void Error_handler(void);

UART_HandleTypeDef huart;  //UART2 handle
TIM_HandleTypeDef htimer2;  //timer2 is general purpose timer

uint32_t pulse1_value = 25000; //to produce 500Hz  (timer_clk_freq/(desired_freq*2)) //slide 85
uint32_t pulse2_value = 12500; //to produce 1000Hz
uint32_t pulse3_value = 6250; //to produce 2000Hz
uint32_t pulse4_value = 3125; //to produce 4000Hz

uint32_t ccr_content;


int main()
{
	HAL_Init();
	SystemCLock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);
	GPIO_Init();

	UART2_Init();
	Timer2_Init();

	uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
	char msg[100];
	sprintf(msg, "Frequency: %ld Hz\r\n", pclk1);
	HAL_UART_Transmit(&huart, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	if (HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_handler();
	}

	if (HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_handler();
	}

	if (HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_3) != HAL_OK)
	{
		Error_handler();
	}

	if (HAL_TIM_OC_Start_IT(&htimer2, TIM_CHANNEL_4) != HAL_OK)
	{
		Error_handler();
	}

	while(1);

	return 0;
}

void SystemCLock_Config_HSE(uint8_t clocl_freq)
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
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_1WS;
		break;
	}
	case SYS_CLOCK_FREQ_84_MHZ:
	{
		osc_init.PLL.PLLM = 4;
		osc_init.PLL.PLLN = 84;
		osc_init.PLL.PLLP = RCC_PLLP_DIV2;
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
		osc_init.PLL.PLLM = 4;
		osc_init.PLL.PLLN = 120;
		osc_init.PLL.PLLP = RCC_PLLP_DIV2;
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
	TIM_OC_InitTypeDef timer2OC_Config;
	htimer2.Instance = TIM2;
	htimer2.Init.Period = 0xFFFFFFFF;
	htimer2.Init.Prescaler = 1;

	if (HAL_TIM_OC_Init(&htimer2) != HAL_OK)
	{
		Error_handler();
	}

	timer2OC_Config.OCMode = TIM_OCMODE_TOGGLE;  //slide 84
	timer2OC_Config.OCPolarity = TIM_OCPOLARITY_HIGH;  //RM >> figure 185: 0: no invertion (TIM_OCPOLARITY_HIGH), 1: invertred signal
	timer2OC_Config.Pulse = pulse1_value;

	if (HAL_TIM_OC_ConfigChannel(&htimer2, &timer2OC_Config, TIM_CHANNEL_1))
	{
		Error_handler();
	}
	timer2OC_Config.Pulse = pulse2_value;

	if (HAL_TIM_OC_ConfigChannel(&htimer2, &timer2OC_Config, TIM_CHANNEL_2))
	{
		Error_handler();
	}

	timer2OC_Config.Pulse = pulse3_value;

	if (HAL_TIM_OC_ConfigChannel(&htimer2, &timer2OC_Config, TIM_CHANNEL_3))
	{
		Error_handler();
	}

	timer2OC_Config.Pulse = pulse4_value;

	if (HAL_TIM_OC_ConfigChannel(&htimer2, &timer2OC_Config, TIM_CHANNEL_4))
	{
		Error_handler();
	}
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, ccr_content + pulse1_value);
	}

	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, ccr_content + pulse2_value);
	}

	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, ccr_content + pulse3_value);
	}

	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	{
		ccr_content = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, ccr_content + pulse4_value);
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
