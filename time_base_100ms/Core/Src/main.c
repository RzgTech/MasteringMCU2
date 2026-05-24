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

void Error_handler(void);



int main()
{
	HAL_Init();
	SystemCLockConfig();

	while(1);

	return 0;
}

void SystemCLockConfig(void)
{

}

void Error_handler(void)
{
	while(1);
}

