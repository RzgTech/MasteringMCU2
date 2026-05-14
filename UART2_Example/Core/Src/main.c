/*
 * main.c
 *
 *  Created on: May 8, 2026
 *      Author: Vahid
 */

void SystemCLockConfig(void);
void UART2_Init(void);
void Error_handler(void);

#include "main.h"
#include "stm32f4xx_hal.h"

UART_HandleTypeDef huart;

int main()
{
	HAL_Init();
	SystemCLockConfig();
	UART2_Init();
	return 0;
}

void SystemCLockConfig(void)
{

}

void UART2_Init(void)
{
	huart.Instance = USART2;
	huart.Init.BaudRate = 115200;
	huart.Init.WordLength = UART_WORDLENGTH_8B;
	huart.Init.StopBits = UART_STOPBITS_1;
	huart.Init.Parity = UART_PARITY_NONE;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.Mode = UART_MODE_TX_RX;

	if (HAL_UART_Init(&huart) != HAL_OK)
	{
		//there is a problem
		Error_handler();
	}
}

void Error_handler(void)
{
	while(1);
}


