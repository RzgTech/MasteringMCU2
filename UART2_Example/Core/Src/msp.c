/*
 * msp.c
 *
 *  Created on: May 8, 2026
 *      Author: Vahid
 */

#include "stm32f4xx_hal.h"


void HAL_MspDeInit(void)
{
	//here we will do low level processor specific inits

		//1. set up priority grouping of the arm cortex mx processor
		HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4); //this is not required bcs by default it is set to NVIC_PRIORITYGROUP_4
		//2. enable the required system exceptions of the arm cortex mx processor
		SCB->SHCSR |= (0x7 << 16);  //enabling mem,bus,usage fault exceptions
		//3. configure the priority for the system exceptions
		HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0); //setting both the preempt and subpriority = 0
		HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
		HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
}
