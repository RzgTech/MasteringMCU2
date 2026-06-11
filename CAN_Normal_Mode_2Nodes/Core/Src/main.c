/*
 * main.c
 *
 *  Created on: Jun 8, 2026
 *      Author: Vahid
 */

#include "main.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdint.h>

void SystemCLock_Config_HSE(uint8_t clocl_freq);
void GPIO_Init(void);
void UART2_Init();
void Error_handler(void);
void CAN1_Init(void);
void CAN_Filter_Config(void);
void CAN1_TX(void);
void Timer6_Init(void);

TIM_HandleTypeDef htimer6;  //timer6 is basic timer
CAN_HandleTypeDef hcan1;
UART_HandleTypeDef huart;  //UART2 handle

int main()
{
	HAL_Init();
	SystemCLock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);
	GPIO_Init();
	UART2_Init();

	Timer6_Init();

	CAN1_Init();
	CAN_Filter_Config();

	//this API helps us to enable the control bits for the intersetd interrupts
	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF) != HAL_OK)
	{
		Error_handler();
	}

	if (HAL_CAN_Start(&hcan1) != HAL_OK)  //based on slide slide 165 (or RM>>30.4.3) we have to change the CAN controller operating mode from Initialization to Normal
	{									  //in order to make it participate in tx and rx activities. O.W txing wont work
		Error_handler();
	}

	while(1);

	return 0;
}

void SystemCLock_Config_HSE(uint8_t clocl_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	uint32_t FLatency = 0;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_HSE;
	osc_init.HSIState = RCC_HSI_ON; //by default it is ON, but anyway, we write it
	osc_init.LSEState = RCC_LSE_ON;
	osc_init.HSEState = RCC_HSE_BYPASS;
	osc_init.HSICalibrationValue = 16;  //by default it is 16, but anyway, we write it
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	osc_init.PLL.PLLState = RCC_PLL_ON;

	switch (clocl_freq)
	{
	case SYS_CLOCK_FREQ_50_MHZ:
	{
		osc_init.PLL.PLLM = 4;
		osc_init.PLL.PLLN = 50;
		osc_init.PLL.PLLP = RCC_PLLP_DIV2;
		osc_init.PLL.PLLQ = 2; //default value
		osc_init.PLL.PLLR = 2; //default value

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_1WS;
		break;
	}
	case SYS_CLOCK_FREQ_84_MHZ:
	{
		osc_init.PLL.PLLM = 4;
		osc_init.PLL.PLLN = 84;
		osc_init.PLL.PLLP = RCC_PLLP_DIV2;
		osc_init.PLL.PLLQ = 2; //default value
		osc_init.PLL.PLLR = 2; //default value

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_2WS;
		break;
	}
	case SYS_CLOCK_FREQ_120_MHZ:
	{
		osc_init.PLL.PLLM = 4;
		osc_init.PLL.PLLN = 120;
		osc_init.PLL.PLLP = RCC_PLLP_DIV2;
		osc_init.PLL.PLLQ = 2; //default value
		osc_init.PLL.PLLR = 2; //default value


		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
				RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_3WS;
		break;
	}
	default:
		return;
	}

	if (HAL_RCC_OscConfig(&osc_init)!= HAL_OK)
	{
		Error_handler();
	}

	if (HAL_RCC_ClockConfig(&clk_init, FLatency))
	{
		Error_handler();
	}

	//Systick configuration
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

void GPIO_Init(void)
{

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio_button;
	GPIO_InitTypeDef gpio_led;

	gpio_button.Pin = GPIO_PIN_13;
	gpio_button.Mode = GPIO_MODE_IT_FALLING;
	gpio_button.Pull = GPIO_NOPULL;
	gpio_button.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &gpio_button);

	gpio_led.Pin = GPIO_PIN_9;
	gpio_led.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_led.Pull = GPIO_NOPULL;
	gpio_led.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &gpio_led);

	gpio_led.Pin = GPIO_PIN_8;
	HAL_GPIO_Init(GPIOC, &gpio_led);

	gpio_led.Pin = GPIO_PIN_8;
	HAL_GPIO_Init(GPIOB, &gpio_led);

	gpio_led.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOC, &gpio_led);

	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
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

void CAN1_Init(void)
{
	hcan1.Instance = CAN1;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.AutoRetransmission = ENABLE;  //trasmission will be retied if it fails (if BUS was not idle). The retransmission wont happen in Loopback mode bcs in loopback mode, ack errors will be ignored
	hcan1.Init.AutoBusOff = ENABLE;
	hcan1.Init.AutoWakeUp = DISABLE;  //for low power mode
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;

	//bit timing settings -> based on http://www.bittiming.can-wiki.info/ , clock rate 25 MHz
	//500 Kbps
	hcan1.Init.Prescaler = 5;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_8TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;

	if (HAL_CAN_Init(&hcan1) != HAL_OK)
	{
		Error_handler();
	}

}

void CAN_Filter_Config(void)
{
	//in this exercise, we will accept all the frames
	CAN_FilterTypeDef can1_filter_config;

	can1_filter_config.FilterActivation = CAN_FILTER_ENABLE;
	can1_filter_config.FilterBank = 0;  //Specifies the filter bank which will be initialized.
										//For single CAN instance(14 dedicated filter banks),
	can1_filter_config.FilterFIFOAssignment = CAN_FILTER_FIFO0; //which FIFO to be used for the message after passing the filter
	can1_filter_config.FilterIdHigh = 0x0000;
	can1_filter_config.FilterIdLow = 0x0000;
	can1_filter_config.FilterMaskIdHigh = 0x0000;
	can1_filter_config.FilterMaskIdLow = 0x0000;
	can1_filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	can1_filter_config.FilterScale = CAN_FILTERSCALE_32BIT; //RM>>30.7.4

	if (HAL_CAN_ConfigFilter(&hcan1, &can1_filter_config) != HAL_OK)
	{
		Error_handler();
	}

}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{

}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

}

uint8_t led_no = 0;
void CAN1_TX(void)
{
	uint32_t TxMailbox;
	CAN_TxHeaderTypeDef TxHeader;
	uint8_t msg;

	TxHeader.IDE = CAN_ID_STD;  //using standard identifier
	TxHeader.StdId = 0x65D;  //sth arbitrary
	TxHeader.DLC = 1;  //length of message
	TxHeader.RTR = CAN_RTR_DATA;

	msg = ++led_no;

	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, &msg, &TxMailbox) != HAL_OK) //we pass TxMailbox to this API to get the mailbox for transmission
	{
		Error_handler();
	}

}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{

}

void Timer6_Init(void)
{
	//It should be noted that, Timers drive clock from APB bus via a multiplier.
	//If the prescaler for APB1 is not 1, multiplicator for the timer is 2, o.w it is 1.
	htimer6.Instance = TIM6;
	//time base of 1 sec
	htimer6.Init.Prescaler = 4999;
	htimer6.Init.Period = 10000-1;
	if (HAL_TIM_Base_Init(&htimer6) != HAL_OK)
	{
		Error_handler();
	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	CAN1_TX();

}

void Error_handler(void)
{
	while(1);
}
