/**
 ******************************************************************************
 * @file	eeprom_24aa16.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-02-18
 * @brief	Functions for the 24AA16, 2048 Byte, EEPROM
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "eeprom_24aa16.h"

#include "FreeRTOS.h"
#include "task.h"

/* Private defines -----------------------------------------------------------*/
#define eepromMS_BETWEEN_WRITES		5

/* Private variables ---------------------------------------------------------*/
portTickType xNextWriteTime;

/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the EEPROM
 * @param	None
 * @retval	None
 */
void EEPROM_Init(EEPROM_Device* Device)
{
	/* Initialize xNextWriteTime - this only needs to be done once. */
	xNextWriteTime = xTaskGetTickCount();
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
	if (Address <= 0x7FF)
	{
		if (Byte != EEPROM_ReadByteFromAddress(Device, Address))
		{
			/* Wait until eepromMS_BETWEEN_WRITES has passed since the last write */
			vTaskDelayUntil(&xNextWriteTime, eepromMS_BETWEEN_WRITES / portTICK_PERIOD_MS);

			EEPROM_I2C_BeginTransmission(Device->Address | (Address >> 8));
			EEPROM_I2C_Write((Address & 0xFF));
			EEPROM_I2C_Write(Byte);
			EEPROM_I2C_EndTransmission();
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
	if (Address <= 0x7FF)
	{
		/* Wait until eepromMS_BETWEEN_WRITES has passed since the last write */
		vTaskDelayUntil(&xNextWriteTime, eepromMS_BETWEEN_WRITES / portTICK_PERIOD_MS);

		EEPROM_I2C_BeginTransmission(Device->Address | (Address >> 8));
		EEPROM_I2C_Write((Address & 0xFF));
		EEPROM_I2C_EndTransmission();
		uint8_t data = 0x00;
		EEPROM_I2C_RequestFrom(Device->Address | (Address >> 8), &data, 1);

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
	if (StartAddress <= 0x7FF)
	{
		/* Wait until eepromMS_BETWEEN_WRITES has passed since the last write */
		vTaskDelayUntil(&xNextWriteTime, eepromMS_BETWEEN_WRITES / portTICK_PERIOD_MS);

		EEPROM_I2C_BeginTransmission(Device->Address | (StartAddress >> 8));
		EEPROM_I2C_Write((StartAddress & 0xFF));
		EEPROM_I2C_EndTransmission();

		EEPROM_I2C_RequestFrom(Device->Address | (StartAddress >> 8), Storage, NumByteToRead);
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}
