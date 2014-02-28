/**
 ******************************************************************************
 * @file	lis331.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-04-04
 * @brief	Contains function prototypes, constants to manage the LIS331 Accelerometer
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LIS331_H_
#define LIS331_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
uint8_t LIS331_Init(uint8_t Address);
void LIS331_GetXYZData(int16_t *X, int16_t *Y, int16_t *Z);

void LIS331_EnableSelfTest();
void LIS331_DisableSelfTest();

#endif /* LIS331_H_ */
