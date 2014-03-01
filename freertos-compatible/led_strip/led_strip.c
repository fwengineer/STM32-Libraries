/**
 ******************************************************************************
 * @file	led_strip.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-02-28
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "led_strip.h"

/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the LED Strip
 * @param	LEDStrip: 	Struct with info about the led strip to configure
 * @retval	None
 */
void LEDStrip_Init(LED_Strip* LEDStrip)
{
	LEDStrip->xSemaphore = xSemaphoreCreateMutex();

	if (LEDStrip->xSemaphore != NULL)
	{
		/* Setup Clocks */
		if (LEDStrip->Timer == TIM2)
		{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		}
		else if (LEDStrip->Timer == TIM3)
		{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		}

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

		/* Setup GPIO */
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		/* Red */
		GPIO_InitStructure.GPIO_Pin = LEDStrip->RedGPIO.GPIO_Pin;
		GPIO_Init(LEDStrip->RedGPIO.GPIO, &GPIO_InitStructure);
		/* Green */
		GPIO_InitStructure.GPIO_Pin =  LEDStrip->GreenGPIO.GPIO_Pin;
		GPIO_Init(LEDStrip->GreenGPIO.GPIO, &GPIO_InitStructure);
		/* Blue */
		GPIO_InitStructure.GPIO_Pin =  LEDStrip->BlueGPIO.GPIO_Pin;
		GPIO_Init(LEDStrip->BlueGPIO.GPIO, &GPIO_InitStructure);
		/* White */
		GPIO_InitStructure.GPIO_Pin =  LEDStrip->WhiteGPIO.GPIO_Pin;
		GPIO_Init(LEDStrip->WhiteGPIO.GPIO, &GPIO_InitStructure);

		/*
		 * Setup TIMx Ch 1-4
		 * TIMxCLK = 72 MHz
		 * TIMx counter clock = TIMxCLK / (Prescaler + 1) = 72 / (2 + 1) MHz = 24 MHz, with prescaler set to 2
		 * Period of 4096 gives 24 MHz / 4096 = 5859.375 Hz Å 5.86 kHz
		 * This period will give 12-bit resolution for the PWM
		 */

		/* Setup the timer */
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		TIM_TimeBaseStructure.TIM_Period 			= ledStripMAX_PERIOD;
		TIM_TimeBaseStructure.TIM_Prescaler 		= 1;
		TIM_TimeBaseStructure.TIM_ClockDivision 	= 0;
		TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up;
		TIM_TimeBaseInit(LEDStrip->Timer, &TIM_TimeBaseStructure);

		/* Setup output compare */
		TIM_OCInitTypeDef TIM_OCInitStructure;
		TIM_OCInitStructure.TIM_OCMode 			= TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState 	= TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse 			= 0;
		TIM_OCInitStructure.TIM_OCPolarity 		= TIM_OCPolarity_High;

		TIM_OC1Init(LEDStrip->Timer, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(LEDStrip->Timer, TIM_OCPreload_Enable);

		TIM_OC2Init(LEDStrip->Timer, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(LEDStrip->Timer, TIM_OCPreload_Enable);

		TIM_OC3Init(LEDStrip->Timer, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(LEDStrip->Timer, TIM_OCPreload_Enable);

		TIM_OC4Init(LEDStrip->Timer, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(LEDStrip->Timer, TIM_OCPreload_Enable);

		TIM_ARRPreloadConfig(LEDStrip->Timer, ENABLE);
		TIM_Cmd(LEDStrip->Timer, ENABLE);
	}
}

/**
 * @brief	Set the colors of the LED Strip
 * @param	LEDStrip: 	Struct with info about the led strip to configure
 * @param	Red: 		Red color value, can be 0 to ledStripMAX_PERIOD
 * @param	Green: 		Green color value, can be 0 to ledStripMAX_PERIOD
 * @param	Blue: 		Blue color value, can be 0 to ledStripMAX_PERIOD
 * @param	White: 		White color value, can be 0 to ledStripMAX_PERIOD
 * @retval	1:			Successfully set the values
 * @retval	0:			One of the values is invalid
 */
uint8_t LEDStrip_SetAll(LED_Strip* LEDStrip, uint16_t Red, uint16_t Green, uint16_t Blue, uint16_t White)
{
	if (Red <= ledStripMAX_PERIOD && Green <= ledStripMAX_PERIOD && Blue <= ledStripMAX_PERIOD && White <= ledStripMAX_PERIOD)
	{
		/* Try to take the semaphore */
		xSemaphoreTake(LEDStrip->xSemaphore, portMAX_DELAY);
		TIM_SetCompare1(LEDStrip->Timer, Red);
		TIM_SetCompare2(LEDStrip->Timer, Green);
		TIM_SetCompare3(LEDStrip->Timer, Blue);
		TIM_SetCompare4(LEDStrip->Timer, White);
		/* Release the semaphore */
		xSemaphoreGive(LEDStrip->xSemaphore);

		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief	Set the colors of the LED Strip by percentage
 * @param	LEDStrip: 	Struct with info about the led strip to configure
 * @param	Red: 		Red color value, can be 0 to 100
 * @param	Green: 		Green color value, can be 0 to 100
 * @param	Blue: 		Blue color value, can be 0 to 100
 * @param	White: 		White color value, can be 0 to 100
 * @retval	1:			Successfully set the values
 * @retval	0:			One of the values is invalid
 */
uint8_t LEDStrip_SetPercentageAll(LED_Strip* LEDStrip, uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t White)
{
	if (Red <= 100 && Green <= 100 && Blue <= 100 && White <= 100)
	{
		/* Try to take the semaphore */
		xSemaphoreTake(LEDStrip->xSemaphore, portMAX_DELAY);
		TIM_SetCompare1(LEDStrip->Timer, Red/100 * ledStripMAX_PERIOD);
		TIM_SetCompare2(LEDStrip->Timer, Green/100 * ledStripMAX_PERIOD);
		TIM_SetCompare3(LEDStrip->Timer, Blue/100 * ledStripMAX_PERIOD);
		TIM_SetCompare4(LEDStrip->Timer, White/100 * ledStripMAX_PERIOD);
		/* Release the semaphore */
		xSemaphoreGive(LEDStrip->xSemaphore);

		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief	Set the colors of the LED Strip by percentage
 * @param	LEDStrip: 	Struct with info about the led strip to configure
 * @param	Color: 		The color to set
 * @param	Value: 		Value of the color, can be 0 to ledStripMAX_PERIOD
 * @retval	1:			Successfully set the values
 * @retval	0:			One of the values is invalid
 */
uint8_t LEDStrip_Set(LED_Strip* LEDStrip, LED_StripColor Color, uint16_t Value)
{
	if (Value <= ledStripMAX_PERIOD && Color <= WHITE)
	{
		switch (Color)
		{
			case RED:
				TIM_SetCompare1(LEDStrip->Timer, Value);
				break;
			case GREEN:
				TIM_SetCompare2(LEDStrip->Timer, Value);
				break;
			case BLUE:
				TIM_SetCompare3(LEDStrip->Timer, Value);
				break;
			case WHITE:
				TIM_SetCompare4(LEDStrip->Timer, Value);
				break;
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief	Check if a color for the LED strip is on
 * @param	LEDStrip: 	Struct with info about the led strip
 * @param	Color: 		The color to check
 * @retval	1:			Color is on
 * @retval	0:			Color is off
 */
uint8_t LEDStrip_ColorIsOn(LED_Strip* LEDStrip, LED_StripColor Color)
{
	uint8_t result = 0;
	switch (Color)
	{
		case RED:
			result = (LEDStrip->Timer->CCR1 != 0);
			break;
		case GREEN:
			result = (LEDStrip->Timer->CCR2 != 0);
			break;
		case BLUE:
			result = (LEDStrip->Timer->CCR3 != 0);
			break;
		case WHITE:
			result = (LEDStrip->Timer->CCR4 != 0);
			break;
	}
	return result;
}
