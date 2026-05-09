/*
 * main.c
 *
 *  Created on: May 8, 2026
 *      Author: Vahid
 */

void SystemCLockConfig(void);

#include "main.h"
#include "stm32f4xx_hal.h"

int main()
{
	HAL_Init();
	SystemCLockConfig();

	return 0;
}

void SystemCLockConfig(void)
{

}
