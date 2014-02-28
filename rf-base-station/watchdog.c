/**
 ******************************************************************************
 * @file	watchdog.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-11-05
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "watchdog.h"
#include "stm32f10x_iwdg.h"
#include "millis/millis.h"
#include "rf-base-station/led.h"

/* Private defines -----------------------------------------------------------*/
#define WATCHDOG_LED	LED_4
/* Private variables ---------------------------------------------------------*/
uint16_t _timeoutValue = 0;
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the Watchdog
 * @param	Timeout: milliseconds before timeout will occur
 * @retval	SUCCESS:
 * @retval	ERROR: Timeout value too high
 */
ErrorStatus WATCHDOG_Init(uint16_t Timeout)
{
	if (Timeout <= 0x0FFF)
	{
		/* Check if the system has resumed from IWDG reset -------------------------*/
		if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
		{
			LED_On(WATCHDOG_LED);

			/* Clear reset flags */
			RCC_ClearFlag();
		}
		else
		{
			LED_Off(WATCHDOG_LED);
		}


		_timeoutValue = Timeout;

		/* Enable write access to IWDG_PR and IWDG_RLR registers */
		IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

		/* IWDG counter clock: 32KHz(LSI) / 32 = 1KHz */
		IWDG_SetPrescaler(IWDG_Prescaler_32);

		/* Set counter reload value */
		IWDG_SetReload(_timeoutValue);

		/* Reload IWDG counter */
		IWDG_ReloadCounter();

		/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
		IWDG_Enable();

		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}

/**
 * @brief	Kick the Watchdog
 * @param	None
 * @retval	None
 */
void WATCHDOG_Kick()
{
	IWDG_ReloadCounter();
}

/**
 * @brief	Check if the Watchdog should be kicked
 * @param	None
 * @retval	None
 */
void WATCHDOG_Check()
{
	static uint32_t watchdogTimer = 0;

	if (millis() - watchdogTimer >= _timeoutValue/2)
	{
		WATCHDOG_Kick();
		watchdogTimer = millis();
	}
}

/* Interrupt Handlers --------------------------------------------------------*/
