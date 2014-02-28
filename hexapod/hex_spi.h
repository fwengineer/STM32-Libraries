/**
 ******************************************************************************
 * @file	hex_spi.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-10
 * @brief	Manage SPI
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HEX_SPI_H_
#define HEX_SPI_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_spi.h"

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void HEX_SPI_Init();
uint8_t HEX_SPI_WriteRead(uint8_t Data);
void HEX_SPI_Write(uint8_t Data);

#endif /* HEX_SPI_H_ */
