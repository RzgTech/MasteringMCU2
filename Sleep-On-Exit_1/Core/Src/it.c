/*
 * it.c
 *
 *  Created on: Jun 8, 2026
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
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_TIM_IRQHandler(&htimer6);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); //we will set and reset to measure the time difference using LA
}
