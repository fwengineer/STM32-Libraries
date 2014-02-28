/**
 ******************************************************************************
 * @file	hex_i2c.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-10
 * @brief	Manage USART
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HEX_I2C_H_
#define HEX_I2C_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void HEX_I2C_Init();

ErrorStatus HEX_I2C_BeginTransmission(uint8_t Address);
void HEX_I2C_EndTransmission();
ErrorStatus HEX_I2C_Write(uint8_t Data);
ErrorStatus HEX_I2C_RequestFrom(uint8_t Address, uint8_t* Storage, uint8_t NumByteToRead);

uint8_t HEX_I2C_SlaveAtAddress(const uint8_t Address);

#endif /* HEX_I2C_H_ */
