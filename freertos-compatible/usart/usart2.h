/**
 ******************************************************************************
 * @file	usart2.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-03-31
 * @brief	Manage USART
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef USART2_H_
#define USART2_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "stm32f10x.h"
#include "outstream/outstream.h"

/* Defines -------------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
OUT_Device USART2_Device;

/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void USART2_Init();

#endif /* USART2_H_ */
