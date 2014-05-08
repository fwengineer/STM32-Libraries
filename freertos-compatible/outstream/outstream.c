/**
 ******************************************************************************
 * @file	outstream.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-07-12
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "outstream.h"

/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Writes data to the OUT Device
 * @param	Device: Pointer to the OUT Device to use
 * @param	Data: data to be written to the OUT Device
 * @retval	None
 */
void OUT_Write(OUT_Device *Device, uint8_t Data)
{
	Device->OUT_WriteFunction(Data);
}

/**
 * @brief	Write a string to the OUT Device
 * @param	Device: Pointer to the OUT Device to use
 * @param	String: The string to write
 * @retval	None
 */
void OUT_WriteString(OUT_Device *Device, const char* String)
{
	while (*String != 0x00)
	{
		OUT_Write(Device, *String);
		String++;
	}
}

/**
 * @brief	Write a single digit as a character to the OUT Device
 * @param	Device: Pointer to the OUT Device to use
 * @param	Digit: The digit to write. Can be 0 to 15 -> 0-F as characters
 * @retval	None
 */
void OUT_WriteDigit(OUT_Device *Device, uint8_t Digit)
{
	if (Digit < 10)
	{
		uint8_t character = 48 + Digit;
		OUT_Write(Device, character);
	}
	else if (Digit < 16)
	{
		uint8_t character = 55 + Digit;
		OUT_Write(Device, character);
	}
}

/**
 * @brief	Write a number as a character to the OUT Device
 * @param	Device: Pointer to the OUT Device to use
 * @param	Number: The number to write, 0 to 4 294 967 295
 * @param	Spaces: Specify if to insert spaces between every third digit
 * @retval	None
 */
void OUT_WriteNumber(OUT_Device *Device, uint32_t Number, uint8_t Spaces)
{
	uint8_t ones, tens, hundreds, thousands, tenThousands, hundredThousands, millions, tenMillions, hundredMillions, billions;

	billions = Number / 1000000000;
	Number = Number % 1000000000;

	hundredMillions = Number / 100000000;
	Number = Number % 100000000;

	tenMillions = Number / 10000000;
	Number = Number % 10000000;

	millions = Number / 1000000;
	Number = Number % 1000000;

	hundredThousands = Number / 100000;
	Number = Number % 100000;

	tenThousands = Number / 10000;
	Number = Number % 10000;

	thousands = Number / 1000;
	Number = Number % 1000;

	hundreds = Number / 100;
	Number = Number % 100;

	tens = Number / 10;
	Number = Number % 10;

	ones = Number;

	if (billions)
	{
		OUT_WriteDigit(Device, (uint8_t)billions);
		if (Spaces) OUT_WriteString(Device, " ");
	}
	if (hundredMillions || billions)
	{
		OUT_WriteDigit(Device, (uint8_t)hundredMillions);
	}
	if (tenMillions || hundredMillions || billions)
	{
		OUT_WriteDigit(Device, (uint8_t)tenMillions);
	}
	if (millions || tenMillions || hundredMillions || billions)
	{
		OUT_WriteDigit(Device, (uint8_t)millions);
		if (Spaces) OUT_WriteString(Device, " ");
	}
	if (hundredThousands || millions || tenMillions || hundredMillions || billions)
	{
		OUT_WriteDigit(Device, (uint8_t)hundredThousands);
	}
	if (tenThousands || hundredThousands || millions || tenMillions || hundredMillions || billions)
	{
		OUT_WriteDigit(Device, (uint8_t)tenThousands);
	}
	if (thousands || tenThousands || hundredThousands || millions || tenMillions || hundredMillions || billions)
	{
		OUT_WriteDigit(Device, (uint8_t)thousands);
		if (Spaces) OUT_WriteString(Device, " ");
	}
	if (hundreds || thousands || tenThousands || hundredThousands || millions || tenMillions || hundredMillions || billions)
	{
		OUT_WriteDigit(Device, (uint8_t)hundreds);
	}
	if (tens || hundreds || thousands || tenThousands || hundredThousands || millions || tenMillions || hundredMillions || billions)
	{
		OUT_WriteDigit(Device, (uint8_t)tens);
	}
	OUT_WriteDigit(Device, (uint8_t)ones);
}

/**
 * @brief	Write a byte formatted as hex to the OUT Device
 * @param	Device: Pointer to the OUT Device to use
 * @param	Byte: The byte to write
 * @param	Prefix: If a "0x" prefix should be added
 * @retval	None
 */
void OUT_WriteHexByte(OUT_Device *Device, uint8_t Byte, uint8_t Prefix)
{
	if (Prefix) OUT_WriteString(Device, "0x");
	OUT_WriteDigit(Device, (Byte >> 4) & 0xF);
	OUT_WriteDigit(Device, Byte & 0xF);
}

/* Interrupt Handlers --------------------------------------------------------*/
