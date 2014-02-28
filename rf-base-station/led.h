/**
 ******************************************************************************
 * @file	led.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-09
 * @brief	Manage the leds on the Hexapod Rev D board
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LED_H_
#define LED_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

/* Defines -------------------------------------------------------------------*/
#define LED_1	GPIO_Pin_10
#define LED_2	GPIO_Pin_11
#define LED_3	GPIO_Pin_12
#define LED_4	GPIO_Pin_15

#ifndef POWER_LED
#define POWER_LED	LED_1
#endif
#ifndef POWER_LED_PERIOD
#define POWER_LED_PERIOD	500
#endif

/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void LED_Init();
void LED_AllOff();
void LED_AllOn();

void LED_On(uint16_t Led);
void LED_Off(uint16_t Led);
void LED_Toggle(uint16_t Led);

void LED_PowerBlink();

void LED_Test();

#endif /* LED_H_ */
