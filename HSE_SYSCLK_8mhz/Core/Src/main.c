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
char msg[100];



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

	clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
			RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;  //RM>>6.3.3 register for prescalers
	clk_init.AHBCLKDivider = RCC_SYSCLK_DIV2;
	clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
	clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
	if (HAL_RCC_ClockConfig(&clk_init, FLASH_ACR_LATENCY_0WS) != HAL_OK)  //flash latency -> RM>>3.4.1
	{
		Error_handler();
	}

	//after this, the system clock will be sourced and powered up by HSE
	//you can disable the HSI:
	__HAL_RCC_HSI_DISABLE(); //saves some current (if HSI is the system clock source, we cannot disable it)

	/*redo the systick configuration*/
	//HAL and APB1 and APB2 frequency have changed. So:
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);  //number of ticks in 1ms, freq = 4MHz
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);  //division by 1 or 8

	UART2_Init();  //also the UART should be reconfigured again bcs the APB1 frequency is chnaged


	memset(msg, 0, sizeof(msg));
	sprintf(msg, "SYSCLK: %ld\r\n", HAL_RCC_GetSysClockFreq());
	HAL_UART_Transmit(&huart, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "HCLK: %ld\r\n", HAL_RCC_GetHCLKFreq());
	HAL_UART_Transmit(&huart, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "PCLK1: %ld\r\n", HAL_RCC_GetPCLK1Freq());
	HAL_UART_Transmit(&huart, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "PCLK2: %ld\r\n", HAL_RCC_GetPCLK2Freq());
	HAL_UART_Transmit(&huart, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

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


