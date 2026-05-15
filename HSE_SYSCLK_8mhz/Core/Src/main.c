/*
 * main.c
 *
 *  Created on: May 8, 2026
 *      Author: Vahid
 */

#include "main.h"
#include <string.h>
#include <stdint.h>

#define FALSE 0
#define TRUE  1

void UART2_Init(void);
void Error_handler(void);

UART_HandleTypeDef huart;



int main()
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	HAL_Init();

	UART2_Init();

	memset(&osc_init, 0, sizeof(osc_init));
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState = RCC_HSE_BYPASS; //in the MCU the clock of the ST-Link circuit is used as the external clock (Bypassing)
	if(HAL_RCC_OscConfig(&osc_init) != HAL_OK)
	{
		Error_handler();
	}


	while(1);

	return 0;
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


