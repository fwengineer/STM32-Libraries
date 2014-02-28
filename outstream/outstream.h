/**
 ******************************************************************************
 * @file	outstream.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-07-12
 * @brief	Manage Outstream Devices. Used to avoid having multiple functions
 * 			with the same code for different peripherals e.g WriteString
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OUTSTREAM_H_
#define OUTSTREAM_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "common.h"

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
typedef struct
{
	char* OUT_DeviceName;
	void (*OUT_WriteFunction)(uint8_t);
	Boolean OUT_Initialized;
} OUT_Device;

/* Function prototypes -------------------------------------------------------*/
void OUT_Write(OUT_Device *Device, uint8_t Data);
void OUT_WriteString(OUT_Device *Device, const char* String);
void OUT_WriteDigit(OUT_Device *Device, uint8_t Digit);
void OUT_WriteNumber(OUT_Device *Device, uint32_t Number, Boolean Spaces);
void OUT_WriteHexByte(OUT_Device *Device, uint8_t Byte, Boolean Prefix);

#endif /* OUTSTREAM_H_ */
