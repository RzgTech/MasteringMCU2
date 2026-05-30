/*
 * msp.c
 *
 *  Created on: May 8, 2026
 *      Author: Vahid
 */

#include "main.h"


void HAL_MspInit(void)
{
	//here we will do low level processor specific inits

		//1. set up priority grouping of the arm cortex mx processor
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4); //this is not required bcs by default it is set to NVIC_PRIORITYGROUP_4
		//2. enable the required system exceptions of the arm cortex mx processor
		SCB->SHCSR |= (0x7 << 16);  //enabling mem,bus,usage fault exceptions
		//3. configure the priority for the system exceptions
		HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0); //making both the preempt and subpriority = 0
		HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
		HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	//enabling the clock for TIM2 and gpioA
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//configuring GPIO to be used as TIM2 input channel
	GPIO_InitTypeDef tim2ch1_gpio;
	tim2ch1_gpio.Mode = GPIO_MODE_AF_PP;
	tim2ch1_gpio.Alternate = GPIO_AF1_TIM2; //from datasheet >> table 11 ->TIM2_CH1
	tim2ch1_gpio.Pin = GPIO_PIN_0;

	HAL_GPIO_Init(GPIOA, &tim2ch1_gpio);

	//setting the priority
	HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);

	//enabling the interrupt
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}


