/**
 ******************************************************************************
 * @file	hex_usart.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-10
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "hex_usart.h"

/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the USART
 * @param	None
 * @retval	None
 */
void HEX_USART_Init()
{
	// Enable clock for Alternate Function IOs
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// Enable remap for USART1 to be able to use it on PB6 and PB7
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

	/* Enable GPIOB clock                                                   */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Configure USART1 Tx (PB6) as alternate function push-pull            */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART1 Rx (PB7) as input floating                         */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	// Enable USART1 Peripheral clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate 				= 115200;
	USART_InitStructure.USART_WordLength 			= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 				= USART_StopBits_1;
	USART_InitStructure.USART_Parity 				= USART_Parity_No;
	USART_InitStructure.USART_Mode 					= USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl 	= USART_HardwareFlowControl_None;
	USART_Init(USART1, &USART_InitStructure);
	// Enable USART1
	USART_Cmd(USART1, ENABLE);
	// Write dummy data cause the first byte is lost for some reason
	HEX_USART_Write(0x00);
}

/**
 * @brief	Writes data to the USART
 * @param	Data: data to be written to the USART
 * @retval	None
 */
void HEX_USART_Write(uint8_t Data)
{
	USART_SendData(USART1, Data);
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

/**
 * @brief	Write a string to the USART
 * @param	String: The string to write
 * @retval	None
 */
void HEX_USART_WriteString(const char* String)
{
	while (*String != 0x00)
	{
		HEX_USART_Write(*String);
		*String++;
	}
}

/**
 * @brief	Write a single digit as a character to the USART
 * @param	Digit: The digit to write. Can be 0 to 15 -> 0-F as characters
 * @retval	None
 */
void HEX_USART_WriteDigit(uint8_t Digit)
{
	if (Digit < 10)
	{
		uint8_t character = 48 + Digit;
		HEX_USART_Write(character);
	}
	else if (Digit < 16)
	{
		uint8_t character = 55 + Digit;
		HEX_USART_Write(character);
	}
}

/**
 * @brief	Write a number as a character to the USB
 * @param	Digit: The digit to write, 0 to 65535
 * @retval	None
 */
void HEX_USART_WriteNumber(uint16_t Number)
{
	uint8_t ones, tens, hundreds, thousands, tenThousands;
	
	tenThousands = Number / 10000;
	Number = Number % 10000;
	
	thousands = Number / 1000;
	Number = Number % 1000;
	
	hundreds = Number / 100;
	Number = Number % 100;
	
	tens = Number / 10;
	Number = Number % 10;
	
	ones = Number;
	
	if (tenThousands) HEX_USART_WriteDigit(tenThousands);
	if (thousands || tenThousands) HEX_USART_WriteDigit(thousands);
	if (hundreds || thousands) HEX_USART_WriteDigit(hundreds);
	if (tens || hundreds) HEX_USART_WriteDigit(tens);
	HEX_USART_WriteDigit(ones);
}

/**
 * @brief	Write a byte formatted as hex to the USART
 * @param	Byte: The byte to write
 * @param	Prefix: If a "0x" prefix should be added
 * @retval	None
 */
void HEX_USART_WriteHexByte(uint8_t Byte, uint8_t Prefix)
{
	if (Prefix) HEX_USART_WriteString("0x");
	HEX_USART_WriteDigit((Byte >> 4) & 0xF);
	HEX_USART_WriteDigit(Byte & 0xF);
}

/* Interrupt Handlers --------------------------------------------------------*/
