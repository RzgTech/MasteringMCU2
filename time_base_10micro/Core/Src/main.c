/*
 * main.c
 *
 *  Created on: May 8, 2026
 *      Author: Vahid
 */

#include "main.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdint.h>

void SystemCLockConfig(uint8_t clocl_freq);
void GPIO_Init(void);
void Error_handler(void);
void Timer6_Init(void);

TIM_HandleTypeDef htimer6;  //timer6 is basic timer


int main()
{
	HAL_Init();
	SystemCLockConfig(SYS_CLOCK_FREQ_50_MHZ);
	GPIO_Init();
	Timer6_Init();

	HAL_TIM_Base_Start_IT(&htimer6);

	while(1);

	return 0;
}

void SystemCLockConfig(uint8_t clocl_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	uint32_t FLatency = 0;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	osc_init.HSIState = RCC_HSI_ON; //by default it is ON, but anyway, we write it
	osc_init.HSICalibrationValue = 16;  //by default it is 16, but anyway, we write it
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI;
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

void Timer6_Init(void)
{
	//It should be noted that, Timers drive clock from APB bus via a multiplier.
	//If the prescaler for APB1 is not 1, multiplicator for the timer is 2, o.w it is 1.
	htimer6.Instance = TIM6;
	htimer6.Init.Prescaler = 9; //slide 54   //only prescaler and period should be used. other elements of the struct are supported by basic timers
	htimer6.Init.Period = 50-1; //slide54,57 //for 100ms-> 64000-1, for 10ms -> 6400-1
	if (HAL_TIM_Base_Init(&htimer6) != HAL_OK)
	{
		Error_handler();
	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6)
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  //Here we are using code( HAL_GPIO_Togglepin() ) to toggle an GPIO whenever update
												//event happens (which can result not taking accurate timings),
												//but a timer peripheral can do that automatically without using any code,
												//for that we have to use OUTPUT COMPARE feature of the timer which is available in
												//general purpose timer. We will explore that when we reach there.
	}
}


void Error_handler(void)
{
	while(1);
}

