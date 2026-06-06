/*
 * it.c
 *
 *  Created on: Jun 6, 2026
 *      Author: Vahid
 */

#include "main.h"

void SysTick_Handler(void)
{
	HAL_IncTick(); //to increment the systick
	HAL_SYSTICK_IRQHandler();  //just calls the callback
}
