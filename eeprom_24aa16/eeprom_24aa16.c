/**
 ******************************************************************************
 * @file	eeprom_24aa16.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-11-05
 * @brief	Functions for the 24AA16, 2048 Byte, EEPROM
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "eeprom_24aa16.h"
//#include "rf-base-station/rf_i2c2.h"
#include <millis/millis.h>
//#include "rf-base-station/led.h"

/* Private defines -----------------------------------------------------------*/
#define TIME_BETWEEN_WRITES		5

/* Private variables ---------------------------------------------------------*/
uint32_t _lastWriteTime;

/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the EEPROM
 * @param	None
 * @retval	None
 */
void EEPROM_Init(EEPROM_Device* Device)
{
	_lastWriteTime = millis();
	Device->Initialized = True;
}


/**
 * @brief	Writes one byte to the specified address
 * @param	Device: Device to write to
 * @param	Byte: Byte to write
 * @param	Address: Address where the byte should be written
 * @retval	SUCCESS: Byte was written
 * @retval	ERROR: Address not valid
 */
ErrorStatus EEPROM_WriteByteToAddress(EEPROM_Device* Device, uint8_t Byte, uint16_t Address)
{
	if (Device->Initialized && Address <= 0x7FF)
	{
		if (Byte != EEPROM_ReadByteFromAddress(Device, Address))
		{
			while (millis() - _lastWriteTime < TIME_BETWEEN_WRITES);

			EEPROM_I2C2_BeginTransmission(Device->Address | (Address >> 8));
			EEPROM_I2C2_Write((Address & 0xFF));
			EEPROM_I2C2_Write(Byte);
			EEPROM_I2C2_EndTransmission();

			_lastWriteTime = millis();
		}
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}

/**
 * @brief	Read one byte from the EEPROM
 * @param	Device: Device to read from
 * @param	Address: Address where the read should be done
 * @retval	The data at the address
 * @retval	ERROR: The address is invalid
 */
uint8_t EEPROM_ReadByteFromAddress(EEPROM_Device* Device, uint16_t Address)
{
	if (Device->Initialized && Address <= 0x7FF)
	{
		while (millis() - _lastWriteTime < TIME_BETWEEN_WRITES);

		EEPROM_I2C2_BeginTransmission(Device->Address | (Address >> 8));
		EEPROM_I2C2_Write((Address & 0xFF));
		EEPROM_I2C2_EndTransmission();
		uint8_t data = 0x00;
		EEPROM_I2C2_RequestFrom(Device->Address | (Address >> 8), &data, 1);

		return data;
	}
	else
	{
		return ERROR;
	}
}

/**
 * @brief	Read one or more bytes from the EEPROM starting at the specified address
 * @param	Device: Device to read from
 * @param	Storage: Pointer to where the data should be stored
 * @param	NumByteToRead: The number of bytes to read
 * @param	StartAddress: Address where the read should start from
 * @retval	SUCCESS: Byte was written
 * @retval	ERROR: Address not valid
 */
ErrorStatus EEPROM_ReadBytesFromStartAddress(EEPROM_Device* Device, uint8_t *Storage, uint16_t NumByteToRead, uint16_t StartAddress)
{
	if (Device->Initialized && StartAddress <= 0x7FF)
	{
		while (millis() - _lastWriteTime < TIME_BETWEEN_WRITES);

		EEPROM_I2C2_BeginTransmission(Device->Address | (StartAddress >> 8));
		EEPROM_I2C2_Write((StartAddress & 0xFF));
		EEPROM_I2C2_EndTransmission();

		EEPROM_I2C2_RequestFrom(Device->Address | (StartAddress >> 8), Storage, NumByteToRead);
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}
