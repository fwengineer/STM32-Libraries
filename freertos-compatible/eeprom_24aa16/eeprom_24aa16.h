/**
 ******************************************************************************
 * @file	eeprom_24aa16.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-02-18
 * @brief
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EEPROM_24AA16_H_
#define EEPROM_24AA16_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "i2c_config.h"

/* Make sure the methods used by the EEPROM is defined */
#if !defined(EEPROM_I2C)
#error "No I2C defined for EEPROM"
#endif

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
typedef struct
{
	uint8_t Address;			/* I2C Address for the EEPROM */

} EEPROM_Device;

/* Function prototypes -------------------------------------------------------*/
void EEPROM_Init(EEPROM_Device* Device);
ErrorStatus EEPROM_WriteByteToAddress(EEPROM_Device* Device, uint8_t Byte, uint16_t Address);

uint8_t EEPROM_ReadByteFromAddress(EEPROM_Device* Device, uint16_t Address);
ErrorStatus EEPROM_ReadBytesFromStartAddress(EEPROM_Device* Device, uint8_t *Storage, uint16_t NumByteToRead, uint16_t StartAddress);


#endif /* EEPROM_24AA16_H_ */
