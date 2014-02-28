/**
 ******************************************************************************
 * @file	wake_up_button.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-10
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "wake_up_button.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "millis/millis.h"
#include "misc.h"
#include "led.h"
#include "rf_usart2_usb.h"


/* Private defines -----------------------------------------------------------*/
#define BUTTON_WAIT_TIME	75
#define BUTTON_PIN			GPIO_Pin_0
#define BUTTON_GPIO			GPIOA

/* Private variables ---------------------------------------------------------*/
uint32_t _wakeUpButtonTimer;
uint8_t _wakeUpButtonPressed;

/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the Wake Up Button
 * @param	None
 * @retval	None
 */
void WAKE_UP_BUTTON_Init()
{
	_wakeUpButtonTimer = millis();
	_wakeUpButtonPressed = False;

	/* Enable GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin 	= BUTTON_PIN;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(BUTTON_GPIO, &GPIO_InitStructure);

	/* Connect EXTI0 Line to PA.00 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line 		= EXTI_Line0;
	EXTI_InitStructure.EXTI_LineCmd 	= ENABLE;
	EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger 	= EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel 						= EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/* Interrupt Handlers --------------------------------------------------------*/
void EXTI0_IRQHandler(void)
{
	static uint32_t lastTimeStamp = 0;

	/* Check if EXTI_Line0 is asserted */
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		if (millis() - lastTimeStamp >= BUTTON_WAIT_TIME)
		{
			lastTimeStamp = millis();
			OUT_WriteString(&RF_USART2_USB, "Button pressed in interrupt!\r");
			LED_Toggle(LED_2);
			LED_Toggle(LED_3);
		}

		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
