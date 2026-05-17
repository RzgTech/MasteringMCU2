/*
 * main.c
 *
 *  Created on: May 8, 2026
 *      Author: Vahid
 */

#include "main.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define FALSE 0
#define TRUE  1

void UART2_Init(void);
void Error_handler(void);


UART_HandleTypeDef huart;



int main()
{

	HAL_Init();

	UART2_Init();



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


