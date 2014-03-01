/**
 ******************************************************************************
 * @file	led_strip.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-02-28
 * @brief
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LED_STRIP_H_
#define LED_STRIP_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32f10x.h"
#include "gpio/gpio.h"

/* Defines -------------------------------------------------------------------*/
#define ledStripMAX_PERIOD		(4095)	/* 4095+1 = 12 bits resolution */

/* Typedefs ------------------------------------------------------------------*/
typedef struct
{
	SemaphoreHandle_t xSemaphore;	/* Mutex for handling multiple multiple task calls */

	TIM_TypeDef* Timer;				/* Timer to use for the led strip */
	GPIO_ConfigTypeDef RedGPIO;		/* GPIO config for red */
	GPIO_ConfigTypeDef GreenGPIO;	/* GPIO config for green */
	GPIO_ConfigTypeDef BlueGPIO;	/* GPIO config for blue */
	GPIO_ConfigTypeDef WhiteGPIO;	/* GPIO config for white */
} LED_Strip;

typedef enum
{
	RED,
	GREEN,
	BLUE,
	WHITE
} LED_StripColor;

/* Function prototypes -------------------------------------------------------*/
void LEDStrip_Init(LED_Strip* LEDStrip);

uint8_t LEDStrip_SetAll(LED_Strip* LEDStrip, uint16_t Red, uint16_t Green, uint16_t Blue, uint16_t White);
uint8_t LEDStrip_SetPercentageAll(LED_Strip* LEDStrip, uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t White);

uint8_t LEDStrip_Set(LED_Strip* LEDStrip, LED_StripColor Color, uint16_t Value);

uint8_t LEDStrip_ColorIsOn(LED_Strip* LEDStrip, LED_StripColor Color);

#endif /* LED_STRIP_H_ */
