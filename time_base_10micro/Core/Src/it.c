/*
 * it.c
 *
 *  Created on: May 8, 2026
 *      Author: Vahid
 */
#include "main.h"
extern TIM_HandleTypeDef htimer6;

void SysTick_Handler(void)
{
	HAL_IncTick(); //to increment the systick
	HAL_SYSTICK_IRQHandler();  //just calls the callback
}

void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htimer6);  //execution of the handler itself might take up to 3 micro sec
								   //so we cannot set any minimum value at ARR
}
