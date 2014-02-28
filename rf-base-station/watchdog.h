/**
 ******************************************************************************
 * @file	watchdog.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-11-05
 * @brief	Manage Watchdog
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WATCHDOG_H_
#define WATCHDOG_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
ErrorStatus WATCHDOG_Init(uint16_t Timeout);
void WATCHDOG_Kick();
void WATCHDOG_Check();

#endif /* WATCHDOG_H_ */
