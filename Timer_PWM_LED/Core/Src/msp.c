/*
 * msp.c
 *
 *  Created on: Jun 1, 2026
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

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uart;
	//here we do the low level initialization of the USART2 peripheral

	//1. enable the clock for the USART2 peripheral as well as for GPIOA peripheral
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	//2. Pin muxing configurations
	gpio_uart.Pin = GPIO_PIN_2;
	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Speed = GPIO_SPEED_FREQ_LOW;
	gpio_uart.Pull = GPIO_PULLUP;
	gpio_uart.Alternate = GPIO_AF7_USART2;  //USART2 TX
											//you should search in stm32f4xx_hal_gpio_ex.h to
										   //find the alt. function of the specific mcu (ex. f446re)
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_uart.Pin = GPIO_PIN_3; //USART2 RX
	HAL_GPIO_Init(GPIOA, &gpio_uart);
	//3. ENable IRQ and set up the priority (NVIC settings)
	HAL_NVIC_EnableIRQ(USART2_IRQn);
	HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);

}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	//enabling the clock for TIM2 and gpioA
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	//configuring GPIO to be used as TIM2 output channel
	/*
	 * datasheet >> table 11:
	 * PA0 -> TIM2_CH1
	 * PA1 -> TIM2_CH2
	 * PA2 -> TIM2_CH3 ----> cannot be used bcs it is used by UART2 instead: PB10
	 * PA3 -> TIM2_CH4 ----> cannot be used bcs it is used by UART2 instead: PB2
	 */

	GPIO_InitTypeDef tim2ch1_gpio;
	tim2ch1_gpio.Mode = GPIO_MODE_AF_PP;
	tim2ch1_gpio.Alternate = GPIO_AF1_TIM2; //from datasheet >> table 11 ->TIM2_CH1
	tim2ch1_gpio.Pull = GPIO_NOPULL;
	tim2ch1_gpio.Speed = GPIO_SPEED_FREQ_LOW;
	tim2ch1_gpio.Pin = GPIO_PIN_0;

	HAL_GPIO_Init(GPIOA, &tim2ch1_gpio);

	//setting the priority
	HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);

	//enabling the interrupt
	HAL_NVIC_EnableIRQ(TIM2_IRQn);


}
