/*
 * it.c
 *
 *  Created on: Jun 8, 2026
 *      Author: Vahid
 */

#include "main.h"
extern TIM_HandleTypeDef htimer6;
extern CAN_HandleTypeDef hcan1;

void SysTick_Handler(void)
{
	HAL_IncTick(); //to increment the systick
	HAL_SYSTICK_IRQHandler();  //just calls the callback
}

void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htimer6);
}

void CAN1_TX_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_RX0_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_RX1_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_SCE_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hcan1);
}

void EXTI15_10_IRQHandler(void)  //for the buuton: when it is pressed, inside the ISR, we will enable the timer6 
{
	HAL_TIM_Base_Start_IT(&htimer6);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

