/**
 ******************************************************************************
 * @file	led.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-09
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_rcc.h"
#include "rf-base-station/led.h"
#include "millis/millis.h"

/* Private defines -----------------------------------------------------------*/
#define LED_GPIO	GPIOA
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the LEDs
 * @param	None
 * @retval	None
 */
void LED_Init()
{
	// Enable clock for Alternate Function IOs
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// Enable remap for JTAG disable to be able to use PA15
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	// Enable clock for GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = LED_1 | LED_2 | LED_3 | LED_4 ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	LED_AllOff();
}


/**
 * @brief	Turn all LEDs off
 * @param	None
 * @retval	None
 */
void LED_AllOff()
{
	GPIO_SetBits(LED_GPIO, LED_1 | LED_2 | LED_3 | LED_4);
}

/**
 * @brief	Turn all LEDs on
 * @param	None
 * @retval	None
 */
void LED_AllOn()
{
	GPIO_ResetBits(LED_GPIO, LED_1 | LED_2 | LED_3 | LED_4);
}

/**
 * @brief	Turn a specified LED on
 * @param	Led: The LED to turn on
 * @retval	None
 */
void LED_On(uint16_t Led)
{
	if (LED_1 == Led || LED_2 == Led || LED_3 == Led || LED_4 == Led)
	{
		GPIO_ResetBits(LED_GPIO, Led);
	}
}

/**
 * @brief	Turn a specified LED off
 * @param	Led: The LED to turn off
 * @retval	None
 */
void LED_Off(uint16_t Led)
{
	if (LED_1 == Led || LED_2 == Led || LED_3 == Led || LED_4 == Led)
	{
		GPIO_SetBits(LED_GPIO, Led);
	}
}

/**
 * @brief	Toggle a specified LED
 * @param	Led: The LED to toggle
 * @retval	None
 */
void LED_Toggle(uint16_t Led)
{
	if (LED_1 == Led || LED_2 == Led || LED_3 == Led || LED_4 == Led)
	{
		LED_GPIO->ODR ^= Led;
	}
}

/**
 * @brief	Blink the power LED defined by POWER_LED every POWER_LED_PERIOD
 * @param	None
 * @retval	None
 */
void LED_PowerBlink()
{
	static uint32_t powerTimer = 0;

	if (millis() - powerTimer >= POWER_LED_PERIOD)
	{
		LED_Toggle(POWER_LED);
		powerTimer = millis();
	}
}

/**
 * @brief	Test the LEDs by blinking them
 * @param	None
 * @retval	None
 */
void LED_Test()
{
	int i;

	LED_On(LED_1);
	for (i=0;i<1000000;i++);
	LED_Off(LED_1);
	for (i=0;i<1000000;i++);

	LED_On(LED_2);
	for (i=0;i<1000000;i++);
	LED_Off(LED_2);
	for (i=0;i<1000000;i++);

	LED_On(LED_3);
	for (i=0;i<1000000;i++);
	LED_Off(LED_3);
	for (i=0;i<1000000;i++);

	LED_On(LED_4);
	for (i=0;i<1000000;i++);
	LED_Off(LED_4);
	for (i=0;i<1000000;i++);
}

/* Interrupt Service Routines ------------------------------------------------*/
