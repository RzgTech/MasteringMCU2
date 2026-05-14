/*
 * main.c
 *
 *  Created on: May 8, 2026
 *      Author: Vahid
 */

#include "main.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdint.h>

void SystemCLockConfig(void);
void UART2_Init(void);
void Error_handler(void);
uint8_t convert_to_capital(uint8_t data);

UART_HandleTypeDef huart;

char *message = "This is a test message\r\n";

int main()
{
	HAL_Init();
	SystemCLockConfig();
	UART2_Init();

	uint16_t len_of_message = strlen(message);
	HAL_UART_Transmit(&huart, (uint8_t *)message, len_of_message, HAL_MAX_DELAY);

	uint8_t rcvd_data;
	uint32_t count = 0;
	uint8_t data_buffer[100];
	while(1)
	{
		HAL_UART_Receive(&huart, &rcvd_data, 1, HAL_MAX_DELAY);
		if (rcvd_data == '\r')
		{
			break;
		}
		else
		{
			data_buffer[count++] = convert_to_capital(rcvd_data);
		}
	}

	data_buffer[count++] = '\r';

	HAL_UART_Transmit(&huart, data_buffer, count, HAL_MAX_DELAY);

	while(1);

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

uint8_t convert_to_capital(uint8_t data)
{
	if (data >= 'a' && data <= 'z')
	{
		data = data - ('a' - 'A');
	}

	return data;
}
