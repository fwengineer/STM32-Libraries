/**
 ******************************************************************************
 * @file	gpio.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-02-28
 * @brief
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GPIO_H_
#define GPIO_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
typedef struct
{
	GPIO_TypeDef* GPIO;			/* GPIO port */
	uint16_t GPIO_Pin;			/* GPIO pin */

} GPIO_ConfigTypeDef;

/* Function prototypes -------------------------------------------------------*/

#endif /* GPIO_H_ */
