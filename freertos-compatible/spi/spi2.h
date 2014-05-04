/**
 ******************************************************************************
 * @file	spi2.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-03-01
 * @brief	Manage SPI2
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SPI2_H_
#define SPI2_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32f10x.h"

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void SPI2_Init();
void SPI2_InitWithStructure(SPI_InitTypeDef* SPI_InitStructure);
uint8_t SPI2_WriteRead(uint8_t Data);

#endif /* SPI2_H_ */
