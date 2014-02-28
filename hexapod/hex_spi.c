/**
 ******************************************************************************
 * @file	hex_spi.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-19
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "hex_spi.h"

/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the SPI
 * @param	None
 * @retval	None
 */
void HEX_SPI_Init()
{
	// Enable GPIOB clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef  GPIO_InitStructure;

	// Configure SPI2-NSS, SPI2-SCK, SPI2-MOSI as output
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Configure SPI2-MISO as input floating
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Enable SPI2 Peripheral clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;						// ???
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							// Hard or soft for chip select???
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	// 24MHz/8 = 3 MHz, Test faster later
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;							// ???
	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_Cmd(SPI2, ENABLE);

	HEX_SPI_WriteRead(0xAA);
}

/**
 * @brief	Writes and receives data from the SPI
 * @param	Data: data to be written to the SPI
 * @retval	The received data
 */
uint8_t HEX_SPI_WriteRead(uint8_t Data)
{
	/* Write in the DR register the data to be sent */
	SPI2->DR = Data;
	// Wait for TX buffer to be empty
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

	/* Return the data in the DR register */
	return (uint8_t)SPI2->DR;
}

/**
 * @brief	Writes data to the SPI
 * @param	Data: data to be written to the SPI
 * @retval	None
 */
void HEX_SPI_Write(uint8_t Data)
{
	/* Write in the DR register the data to be sent */
	SPI2->DR = Data;
	// Wait for TX buffer to be empty
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
}


/* Interrupt Handlers --------------------------------------------------------*/
