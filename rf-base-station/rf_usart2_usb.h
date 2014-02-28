/**
 ******************************************************************************
 * @file	rf_usart2_usb.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-10
 * @brief	Manage USART
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RF_USART2_USB_H_
#define RF_USART2_USB_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "input-output/outstream.h"

/* Defines -------------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
OUT_Device RF_USART2_USB;

/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void RF_USART2_USB_Init();

#endif /* RF_USART2_USB_H_ */
