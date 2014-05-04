/**
 ******************************************************************************
 * @file	spi1.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-03-27
 * @brief	Manage SPI1
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SPI1_H_
#define SPI1_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32f10x.h"

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void SPI1_Init();
void SPI1_InitWithStructure(SPI_InitTypeDef* SPI_InitStructure);
uint8_t SPI1_WriteRead(uint8_t Data);

#endif /* SPI1_H_ */
