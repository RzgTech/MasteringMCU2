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

void Error_handler(void);
void Timer6_Init(void);

TIM_HandleTypeDef htimer6;  //timer6 is basic timer


int main()
{
	HAL_Init();
	SystemCLockConfig();
	Timer6_Init();


	while(1);

	return 0;
}

void SystemCLockConfig(void)
{

}

void Timer6_Init(void)
{
	//It should be noted that, Timers drive clock from APB bus via a multiplier.
	//If the prescaler for APB1 is not 1, multiplicator for the timer is 2, o.w it is 1.
	htimer6.Instance = TIM6;
	htimer6.Init.Prescaler = 24; //slide 54   //only prescaler and period should be used. other elements of the struct are supported by basic timers
	htimer6.Init.Period = 64000-1; //slide54,57
	if (HAL_TIM_Base_Init(&htimer6) != HAL_OK)
	{
		Error_handler();
	}

}

void Error_handler(void)
{
	while(1);
}

