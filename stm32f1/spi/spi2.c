/**
 ******************************************************************************
 * @file	spi2.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-03-01
 * @brief
 * @note	If a spi function is called from an IRQ, that IRQ has to have a
 * 			lower priority than the SPI IRQ
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "spi2.h"

/* Private defines -----------------------------------------------------------*/
#define SPI_NO		2
#define SPIx		(SPI2)
#define SPIx_IRQn	(SPI2_IRQn)
#define SCK_Pin		(GPIO_Pin_13)
#define MISO_Pin	(GPIO_Pin_14)
#define MOSI_Pin	(GPIO_Pin_15)

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the SPI
 * @param	None
 * @retval	None
 */
void SPI2_Init()
{
	/* Enable GPIOx clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* Configure SPIx-SCK, SPIx-MOSI alternate function push-pull */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   			= SCK_Pin | MOSI_Pin;
	GPIO_InitStructure.GPIO_Speed 			= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  			= GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure SPIx-MISO input floating */
	GPIO_InitStructure.GPIO_Pin   			= MISO_Pin;
	GPIO_InitStructure.GPIO_Speed 			= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  			= GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable SPIx Peripheral clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	/* Initialize SPIx */
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;
	SPI_Init(SPIx, &SPI_InitStructure);

	/* Enable SPIx */
	SPI_Cmd(SPIx, ENABLE);
}

/**
 * @brief	Writes and receives data from the SPI
 * @param	Data: data to be written to the SPI
 * @retval	The received data
 */
uint8_t SPI2_WriteRead(uint8_t Data)
{
	/* Loop while DR register is not empty */
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);

	/* Send byte through the SPIx peripheral */
	SPI_I2S_SendData(SPIx, (uint16_t)Data);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPIx);
}

/**
 * @brief	Writes data to the SPI
 * @param	Data: data to be written to the SPI
 * @retval	None
 */
void SPI2_Write(uint8_t Data)
{
	/* Loop while DR register is not empty */
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);

	/* Send byte through the SPIx peripheral */
	SPI_I2S_SendData(SPIx, (uint16_t)Data);
}


/* Interrupt Handlers --------------------------------------------------------*/
