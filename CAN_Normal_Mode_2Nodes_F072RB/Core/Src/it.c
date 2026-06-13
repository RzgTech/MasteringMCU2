/*
 * it.c
 *
 *  Created on: Jun 8, 2026
 *      Author: Vahid
 */

#include <main_app.h>
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

void CEC_CAN_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hcan1);
}

void EXTI4_15_IRQHandler(void)  //for the buuton: when it is pressed, inside the ISR, we will enable the timer6
{
	HAL_TIM_Base_Start_IT(&htimer6);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

