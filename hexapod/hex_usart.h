/**
 ******************************************************************************
 * @file	hex_usart.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-10
 * @brief	Manage USART
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HEX_USART_H_
#define HEX_USART_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_usart.h"

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void HEX_USART_Init();
void HEX_USART_Write(uint8_t Data);
void HEX_USART_WriteString(const char* String);
void HEX_USART_WriteDigit(uint8_t Digit);
void HEX_USART_WriteNumber(uint16_t Number);
void HEX_USART_WriteHexByte(uint8_t Byte, uint8_t Prefix);

#endif /* HEX_USART_H_ */
