/**
 ******************************************************************************
 * @file	rf_i2c2.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-10
 * @brief	Manage USART
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RF_I2C2_H_
#define RF_I2C2_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void RF_I2C2_Init();

ErrorStatus RF_I2C2_BeginTransmission(uint8_t Address);
void RF_I2C2_EndTransmission();
ErrorStatus RF_I2C2_Write(uint8_t Data);
ErrorStatus RF_I2C2_RequestFrom(uint8_t Address, uint8_t* Storage, uint16_t NumByteToRead);

uint8_t RF_I2C2_SlaveAtAddress(const uint8_t Address);

#endif /* RF_I2C2_H_ */
