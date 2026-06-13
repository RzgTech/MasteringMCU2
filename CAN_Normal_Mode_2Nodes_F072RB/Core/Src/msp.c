/*
 * msp.c
 *
 *  Created on: Jun 8, 2026
 *      Author: Vahid
 */
#include "main_app.h"


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htimer)
{
	//1. Enabling TIM6 clock
	__HAL_RCC_TIM6_CLK_ENABLE();

	//2. ENabling IRQ
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	//3. Configure the priority
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 15, 0);
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
	gpio_uart.Alternate = GPIO_AF1_USART2;  //USART2 TX
											//you should search in stm32f4xx_hal_gpio_ex.h to
										   //find the alt. function of the specific mcu (ex. f446re)
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_uart.Pin = GPIO_PIN_3; //USART2 RX
	HAL_GPIO_Init(GPIOA, &gpio_uart);
	//3. ENable IRQ and set up the priority (NVIC settings)
	HAL_NVIC_EnableIRQ(USART2_IRQn);
	HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);

}

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
	GPIO_InitTypeDef gpio_can;

	__HAL_RCC_CAN1_CLK_ENABLE();

	gpio_can.Pin = GPIO_PIN_11;  //CAN1_RX
	gpio_can.Mode = GPIO_MODE_AF_PP;
	gpio_can.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_can.Pull = GPIO_PULLUP;
	gpio_can.Alternate = GPIO_AF4_CAN;

	HAL_GPIO_Init(GPIOA, &gpio_can);

	gpio_can.Pin = GPIO_PIN_12;  //CAN1_TX
	HAL_GPIO_Init(GPIOA, &gpio_can);

	//setting interrupt priorities
	HAL_NVIC_SetPriority(CEC_CAN_IRQn, 0, 0);

	//enabling the interrupts
	HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);


}
