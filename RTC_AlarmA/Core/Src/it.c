/*
 * it.c
 *
 *  Created on: Jun 8, 2026
 *      Author: Vahid
 */

#include <main_app.h>

extern RTC_HandleTypeDef hrtc;

void SysTick_Handler(void)
{
	HAL_IncTick(); //to increment the systick
	HAL_SYSTICK_IRQHandler();  //just calls the callback
}

void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void RTC_Alarm_IRQHandler(void)
{
	HAL_RTC_AlarmIRQHandler(&hrtc);
}
