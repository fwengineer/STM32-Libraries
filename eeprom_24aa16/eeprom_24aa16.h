/**
 ******************************************************************************
 * @file	eeprom_24aa16.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-11-05
 * @brief
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EEPROM_24AA16_H_
#define EEPROM_24AA16_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <common.h>

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
typedef struct
{
	uint8_t Address;			/* I2C Address for the EEPROM */
	Boolean Initialized;		/* True if initialized, False otherwise */

} EEPROM_Device;

/* Function prototypes -------------------------------------------------------*/
void EEPROM_Init(EEPROM_Device* Device);
ErrorStatus EEPROM_WriteByteToAddress(EEPROM_Device* Device, uint8_t Byte, uint16_t Address);

uint8_t EEPROM_ReadByteFromAddress(EEPROM_Device* Device, uint16_t Address);
ErrorStatus EEPROM_ReadBytesFromStartAddress(EEPROM_Device* Device, uint8_t *Storage, uint16_t NumByteToRead, uint16_t StartAddress);


#endif /* EEPROM_24AA16_H_ */
