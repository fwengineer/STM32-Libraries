/**
 ******************************************************************************
 * @file	spi.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-05-04
 * @brief
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* Private defines -----------------------------------------------------------*/
#define SCK_Pin_1	(GPIO_Pin_5)
#define MISO_Pin_1	(GPIO_Pin_6)
#define MOSI_Pin_1	(GPIO_Pin_7)

#define SCK_Pin_2	(GPIO_Pin_13)
#define MISO_Pin_2	(GPIO_Pin_14)
#define MOSI_Pin_2	(GPIO_Pin_15)

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the SPI
 * @param	None
 * @retval	None
 */
void SPI_Device_Init(SPI_Device* SPIDevice)
{
	/* Initialize SPIx */
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;

	SPI_InitWithStructure(SPIDevice, &SPI_InitStructure);
}

/**
 * @brief	Initializes the SPI
 * @param	SPI_InitStructure: Struct with the parameters for the SPI peripheral
 * @retval	None
 */
void SPI_InitWithStructure(SPI_Device* SPIDevice, SPI_InitTypeDef* SPI_InitStructure)
{
	SPIDevice->receivedByte = 0;

	/*
	 * Create the binary semaphores:
	 * The semaphore is created in the 'empty' state, meaning
	 * the semaphore must first be given before it can be taken (obtained)
	 * using the xSemaphoreTake() function.
	 */
	SPIDevice->xTxSemaphore = xSemaphoreCreateBinary();
	SPIDevice->xRxSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(SPIDevice->xTxSemaphore);				/* We know TX should be empty from the beginning */
	xSemaphoreGive(SPIDevice->xRxSemaphore);
	xSemaphoreTake(SPIDevice->xRxSemaphore, portMAX_DELAY);

	if (SPIDevice->SPI_Channel == 1)
	{
		/* Enable GPIOx clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

		/* NVIC Configuration */
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel 						= SPI1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		/* Configure SPIx-SCK, SPIx-MOSI alternate function push-pull */
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin   			= SCK_Pin_1 | MOSI_Pin_1;
		GPIO_InitStructure.GPIO_Speed 			= GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode  			= GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Configure SPIx-MISO input floating */
		GPIO_InitStructure.GPIO_Pin   			= MISO_Pin_1;
		GPIO_InitStructure.GPIO_Speed 			= GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode  			= GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);


		/* Enable SPIx Peripheral clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	}
	else if (SPIDevice->SPI_Channel == 2)
	{
		/* Enable GPIOx clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

		/* NVIC Configuration */
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel 						= SPI2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		/* Configure SPIx-SCK, SPIx-MOSI alternate function push-pull */
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin   			= SCK_Pin_2 | MOSI_Pin_2;
		GPIO_InitStructure.GPIO_Speed 			= GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode  			= GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* Configure SPIx-MISO input floating */
		GPIO_InitStructure.GPIO_Pin   			= MISO_Pin_2;
		GPIO_InitStructure.GPIO_Speed 			= GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode  			= GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* Enable SPIx Peripheral clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	}

	/* Initialize SPIx */
	SPI_Init(SPIDevice->SPIx, SPI_InitStructure);

	/*
	 * Enable SPI_I2S_IT_RXNE interrupt
	 * SPI_I2S_IT_TXE interrupt will only be enabled when a write should happen
	 * */
	SPI_I2S_ITConfig(SPIDevice->SPIx, SPI_I2S_IT_RXNE, ENABLE);

	/* Enable SPIx */
	SPI_Cmd(SPIDevice->SPIx, ENABLE);
}

/**
 * @brief	Writes and receives data from the SPI
 * @param	Data: data to be written to the SPI
 * @retval	The received data
 */
uint8_t SPI_WriteRead(SPI_Device* SPIDevice, uint8_t Data)
{
	/* Enable SPI_MASTER TXE interrupt */
	SPI_I2S_ITConfig(SPIDevice->SPIx, SPI_I2S_IT_TXE, ENABLE);
	/* Try to take the TX Semaphore */
	xSemaphoreTake(SPIDevice->xTxSemaphore, portMAX_DELAY);

	/* Send byte through the SPIx peripheral */
	SPIDevice->SPIx->DR = Data;

	/* Try to take the RX Semaphore */
	xSemaphoreTake(SPIDevice->xRxSemaphore, portMAX_DELAY);

	/* Return the byte read from the SPI bus */
	return SPIDevice->receivedByte;
}

/* Interrupt Handlers --------------------------------------------------------*/
void SPI_Interrupt(SPI_Device* SPIDevice)
{
	/* Transmit buffer empty interrupt */
	if (SPI_I2S_GetITStatus(SPIDevice->SPIx, SPI_I2S_IT_TXE) != RESET)
	{
		/* Release the semaphore */
		xSemaphoreGiveFromISR(SPIDevice->xTxSemaphore, NULL);

		/* Disable SPI_MASTER TXE interrupt */
		SPI_I2S_ITConfig(SPIDevice->SPIx, SPI_I2S_IT_TXE, DISABLE);
	}
	/* Receive buffer not empty interrupt */
	else if (SPI_I2S_GetITStatus(SPIDevice->SPIx, SPI_I2S_IT_RXNE) != RESET)
	{
		/* Release the semaphore */
		xSemaphoreGiveFromISR(SPIDevice->xRxSemaphore, NULL);

		/* Read the byte received in order to clear the interrupt flag */
		SPIDevice->receivedByte = SPI_I2S_ReceiveData(SPIDevice->SPIx);
	}
}
