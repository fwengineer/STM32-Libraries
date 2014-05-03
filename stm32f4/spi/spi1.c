/**
 ******************************************************************************
 * @file	spi1.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-04-1
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "spi1.h"

/* Private defines -----------------------------------------------------------*/
#define SPI_NO		1
#define SPIx		(SPI1)
#define SPIx_IRQn	(SPI1_IRQn)
#define SCK_Pin		(GPIO_Pin_5)
#define MISO_Pin	(GPIO_Pin_6)
#define MOSI_Pin	(GPIO_Pin_7)

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the SPI
 * @param	None
 * @retval	None
 */
void SPI1_Init()
{
	/* Enable GPIOx clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Configure SPIx-SCK, SPIx-MOSI alternate function push-pull */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   			= SCK_Pin | MOSI_Pin;
	GPIO_InitStructure.GPIO_Speed 			= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  			= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType			= GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure SPIx-MISO input floating */
	GPIO_InitStructure.GPIO_Pin   			= MISO_Pin;
	GPIO_InitStructure.GPIO_Speed 			= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  			= GPIO_Mode_IN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure alternate function */
	GPIO_PinAFConfig(GPIOA, SCK_Pin | MOSI_Pin | MISO_Pin, GPIO_AF_SPI1);

	/* Enable SPIx Peripheral clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* Initialize SPIx */
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
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
uint8_t SPI1_WriteRead(uint8_t Data)
{
#if 0 /* This adds overhead */
	/* Loop while DR register is not empty */
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);

	/* Send byte through the SPIx peripheral */
	SPI_I2S_SendData(SPIx, (uint16_t)Data);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPIx);
#endif

#if 1 /* This is faster */
	/* Loop while DR register is not empty */
	while ((SPIx->SR & SPI_I2S_FLAG_TXE) == RESET);
	/* Send byte through the SPIx peripheral */
	SPIx->DR = Data;
	/* Wait to receive a byte */
	while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == RESET);
	return SPIx->DR;
#endif
}

/**
 * @brief	Writes data to the SPI
 * @param	Data: data to be written to the SPI
 * @retval	None
 */
void SPI1_Write(uint8_t Data)
{
#if 0 /* This adds overhead */
	/* Loop while DR register is not empty */
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	/* Send byte through the SPIx peripheral */
	SPI_I2S_SendData(SPIx, (uint16_t)Data);
#endif

#if 1 /* This is faster */
	/* Loop while DR register is not empty */
	while ((SPIx->SR & SPI_I2S_FLAG_TXE) == RESET);
	/* Send byte through the SPIx peripheral */
	SPIx->DR = Data;
#endif
}

/* Interrupt Handlers --------------------------------------------------------*/
