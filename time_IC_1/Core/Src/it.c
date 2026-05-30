/*
 * it.c
 *
 *  Created on: May 8, 2026
 *      Author: Vahid
 */
#include "main.h"
extern TIM_HandleTypeDef htimer2;

void SysTick_Handler(void)
{
	HAL_IncTick(); //to increment the systick
	HAL_SYSTICK_IRQHandler();  //just calls the callback
}

void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htimer2);
}
