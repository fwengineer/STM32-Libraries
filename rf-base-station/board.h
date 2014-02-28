/**
 ******************************************************************************
 * @file	board.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-25
 * @brief	Boardspecific code the libraries use to get peripheral functions
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BOARD_H_
#define BOARD_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#include "common.h"
#include "input-output/outstream.h"

#include "rf_usart2_usb.h"
#include "rf_usart1.h"
#include "rf_i2c2.h"
#include "rf_spi1.h"
#include "rf_spi2.h"

#include "nrf24l01/nrf24l01.h"
#include "nrf24l01/nrf24l01_register_map.h"

#include "eeprom_24aa16/eeprom_24aa16.h"
#include "watchdog.h"
#include "wake_up_button.h"
#include "led.h"

/* Defines -------------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
NRF24L01_Device NRF24L01_1;
NRF24L01_Device NRF24L01_2;

EEPROM_Device EEPROM;

/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void BOARD_Init()
{
	LED_Init();
	LED_AllOff();

	RF_USART2_USB_Init();
	RF_USART1_Init();
	RF_I2C2_Init();

	EEPROM.Address = 0x50;
	EEPROM_Init(&EEPROM);

#ifdef NRF24L01
	/* nRF24L01_1 */
	NRF24L01_1.NRF24L01_DeviceName	= "1";
	NRF24L01_1.CSN_Pin 				= GPIO_Pin_4;
	NRF24L01_1.CSN_GPIO				= GPIOA;
	NRF24L01_1.CE_Pin 				= GPIO_Pin_0;
	NRF24L01_1.CE_GPIO				= GPIOB;

	NRF24L01_1.IRQ_Pin 				= GPIO_Pin_1;
	NRF24L01_1.IRQ_GPIO				= GPIOB;
	NRF24L01_1.IRQ_GPIO_PortSource 	= GPIO_PortSourceGPIOB;
	NRF24L01_1.IRQ_GPIO_PinSource 	= GPIO_PinSource1;
	NRF24L01_1.IRQ_EXTI_Line 		= EXTI_Line1;
	NRF24L01_1.IRQ_NVIC_IRQChannel 	= EXTI1_IRQn;

	NRF24L01_1.SPIx 				= SPI1;
	NRF24L01_1.SPIx_Init 			= RF_SPI1_Init;
	NRF24L01_1.SPIx_WriteRead 		= RF_SPI1_WriteRead;
	NRF24L01_1.SPIx_Write 			= RF_SPI1_Write;
	NRF24L01_Init(&NRF24L01_1);

	NRF24L01_SetRxPipeAddress(&NRF24L01_1, 0, DEVICE_0_1_ADDRESS);	// The device should have it's own address on pipe 0
	NRF24L01_SetRxPipeAddress(&NRF24L01_1, 1, DEVICE_1_1_ADDRESS);
	NRF24L01_SetRxPipeAddress(&NRF24L01_1, 2, DEVICE_1_2_ADDRESS);
	NRF24L01_SetRxPipeAddress(&NRF24L01_1, 3, DEVICE_1_3_ADDRESS);
	NRF24L01_SetRxPipeAddress(&NRF24L01_1, 4, DEVICE_1_4_ADDRESS);
	NRF24L01_SetRxPipeAddress(&NRF24L01_1, 5, DEVICE_1_5_ADDRESS);

	NRF24L01_SetTxAddress(&NRF24L01_1, DEVICE_0_1_ADDRESS);	// Will change depending on who you want to talk to

	/* nRF24L01_2 */
	NRF24L01_2.NRF24L01_DeviceName	= "2";
	NRF24L01_2.CSN_Pin 				= GPIO_Pin_12;
	NRF24L01_2.CSN_GPIO				= GPIOB;
	NRF24L01_2.CE_Pin 				= GPIO_Pin_8;
	NRF24L01_2.CE_GPIO				= GPIOA;

	NRF24L01_2.IRQ_Pin 				= GPIO_Pin_9;
	NRF24L01_2.IRQ_GPIO				= GPIOA;
	NRF24L01_2.IRQ_GPIO_PortSource 	= GPIO_PortSourceGPIOA;
	NRF24L01_2.IRQ_GPIO_PinSource 	= GPIO_PinSource9;
	NRF24L01_2.IRQ_EXTI_Line 		= EXTI_Line9;
	NRF24L01_2.IRQ_NVIC_IRQChannel 	= EXTI9_5_IRQn;

	NRF24L01_2.SPIx 				= SPI2;
	NRF24L01_2.SPIx_Init 			= RF_SPI2_Init;
	NRF24L01_2.SPIx_WriteRead 		= RF_SPI2_WriteRead;
	NRF24L01_2.SPIx_Write 			= RF_SPI2_Write;
	NRF24L01_Init(&NRF24L01_2);

	NRF24L01_SetTxAddress(&NRF24L01_2, DEVICE_1_4_ADDRESS);
#endif /* NRF24L01 */

	WAKE_UP_BUTTON_Init();

	WATCHDOG_Init(2000);
}


/* Interrupt Service Routines ------------------------------------------------*/
void EXTI1_IRQHandler(void)
{
	/* Check if EXTI_Line1 is asserted */
	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		//OUT_WriteString(&RF_USART2_USB, "nRF24L01_1 Interrupt\r");
		LED_Toggle(LED_3);

		NRF24L01_Interrupt(&NRF24L01_1);

		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

void EXTI9_5_IRQHandler(void)
{
	/* Check if EXTI_Line9 is asserted */
	if (EXTI_GetITStatus(EXTI_Line9) != RESET)
	{
		//OUT_WriteString(&RF_USART2_USB, "nRF24L01_2 Interrupt\r");
		LED_Toggle(LED_2);

		NRF24L01_Interrupt(&NRF24L01_2);

		/* Clear the EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}

#endif /* BOARD_H_ */
