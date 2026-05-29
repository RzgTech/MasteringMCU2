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
