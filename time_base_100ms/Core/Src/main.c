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
void Timer6_Init(void)

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
	htimer6.Instance = TIM6;
	htimer6.Init =   //only prescaler and period should be used. other elements of the struct are supported by basic timers

}

void Error_handler(void)
{
	while(1);
}

