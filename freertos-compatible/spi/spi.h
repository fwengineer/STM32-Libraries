/**
 ******************************************************************************
 * @file	spi.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-05-04
 * @brief	Manage SPI
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SPI_H_
#define SPI_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32f10x.h"

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
typedef struct
{
	uint8_t SPI_Channel;		/* Channel for the SPI periperal */
	SPI_TypeDef* SPIx;			/* SPI peripheral to use */

	uint8_t receivedByte;

	SemaphoreHandle_t xTxSemaphore;
	SemaphoreHandle_t xRxSemaphore;
} SPI_Device;

/* Function prototypes -------------------------------------------------------*/
void SPI_Device_Init(SPI_Device* SPIDevice);
void SPI_InitWithStructure(SPI_Device* SPIDevice, SPI_InitTypeDef* SPI_InitStructure);
uint8_t SPI_WriteRead(SPI_Device* SPIDevice, uint8_t Data);

void SPI_Interrupt(SPI_Device* SPIDevice);

#endif /* SPI_H_ */
