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

void SystemCLockConfig(void);
void GPIO_Init(void);
void Error_handler(void);
void Timer6_Init(void);

TIM_HandleTypeDef htimer6;  //timer6 is basic timer


int main()
{
	HAL_Init();
	SystemCLockConfig();
	GPIO_Init();
	Timer6_Init();
	HAL_TIM_Base_Start(&htimer6);
	while(1)
	{
		//loop until the update event flag is set
		while (! (TIM6->SR & TIM_SR_UIF)); //when we are out of this loop it means 100 ms is passed

		TIM6->SR = 0;  //clearing UIF bit
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

	}


	while(1);

	return 0;
}

void SystemCLockConfig(void)
{

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
	htimer6.Init.Prescaler = 24; //slide 54   //only prescaler and period should be used. other elements of the struct are supported by basic timers
	htimer6.Init.Period = 64000-1; //slide54,57 //for 100ms-> 64000-1, for 10ms -> 6400-1
	if (HAL_TIM_Base_Init(&htimer6) != HAL_OK)
	{
		Error_handler();
	}

}

void Error_handler(void)
{
	while(1);
}

