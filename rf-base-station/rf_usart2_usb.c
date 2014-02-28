/**
 ******************************************************************************
 * @file	rf_usart2_usb.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-10
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "rf_usart2_usb.h"

/* Private defines -----------------------------------------------------------*/
#define USARTx		USART2
#define DEVICE		RF_USART2_USB

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief	Writes data to the USART
 * @param	Data: data to be written to the USART
 * @retval	None
 */
void RF_USART2_USB_WriteRaw(uint8_t Data)
{
	USART_SendData(USARTx, Data);
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the USART
 * @param	None
 * @retval	None
 */
void RF_USART2_USB_Init()
{
	/* Initialize the Out Device */
	DEVICE.OUT_DeviceName = "RF_USART2_USB";
	DEVICE.OUT_WriteFunction = RF_USART2_USB_WriteRaw;
	DEVICE.OUT_Initialized = True;

	/* Enable clock for Alternate Function IOs */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* Enable GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Configure USART2 Tx (PA2) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART2 Rx (PA3) as input floating */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Enable USART2 Peripheral clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate 				= 115200;
	USART_InitStructure.USART_WordLength 			= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 				= USART_StopBits_1;
	USART_InitStructure.USART_Parity 				= USART_Parity_No;
	USART_InitStructure.USART_Mode 					= USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl 	= USART_HardwareFlowControl_None;
	USART_Init(USARTx, &USART_InitStructure);

	/* Enable the USART2 Interrupt */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel 						= USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable RXNE Interrupt */
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);

	/* Enable USART2 */
	USART_Cmd(USARTx, ENABLE);

	/* Write dummy data cause the first byte is lost for some reason */
	OUT_Write(&DEVICE, 0x00);
	OUT_WriteString(&DEVICE, DEVICE.OUT_DeviceName);
	OUT_WriteString(&DEVICE, " - Initialized\r");
}

/* Interrupt Handlers --------------------------------------------------------*/
void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USARTx, USART_IT_RXNE) == SET)
	{
		uint8_t data = (uint8_t)USART_ReceiveData(USARTx);
		OUT_Write(&DEVICE, data);
	}
}
