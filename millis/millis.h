/**
 ******************************************************************************
 * @file	millis.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-09
 * @brief	Manage a millis counter
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MILLIS_H_
#define MILLIS_H_

/* Includes ------------------------------------------------------------------*/
#if defined(STM32F10X_MD_VL) || defined(STM32F10X_MD)
#include "stm32f10x.h"
#endif
#if defined(STM32F40_41xxx)
#include "stm32f4xx.h"
#endif
/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void MILLIS_Init(void);
uint32_t millis(void);
void millisDelay(uint32_t);

#endif /* MILLIS_H_ */
