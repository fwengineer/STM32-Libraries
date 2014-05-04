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

/* TODO: Fix overhead issues from the standard library functions */

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
SemaphoreHandle_t xTxSemaphore_2 = NULL;
SemaphoreHandle_t xRxSemaphore_2 = NULL;

uint8_t receivedByte_2 = 0;

/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the SPI
 * @param	None
 * @retval	None
 */
void SPI2_Init()
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

	SPI2_InitWithStructure(&SPI_InitStructure);
}

/**
 * @brief	Initializes the SPI
 * @param	SPI_InitStructure: Struct with the parameters for the SPI peripheral
 * @retval	None
 */
void SPI2_InitWithStructure(SPI_InitTypeDef* SPI_InitStructure)
{
	/*
	 * Create the binary semaphores:
	 * The semaphore is created in the 'empty' state, meaning
	 * the semaphore must first be given before it can be taken (obtained)
	 * using the xSemaphoreTake() function.
	 */
	xTxSemaphore_2 = xSemaphoreCreateBinary();
	xRxSemaphore_2 = xSemaphoreCreateBinary();
	/* TODO: Check if this is needed because the semaphore has to be given before it can be taken */
//	xSemaphoreGive(xTxSemaphore_1);

	/* Enable GPIOx clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* NVIC Configuration */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel 						= SPIx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);

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
	SPI_Init(SPIx, SPI_InitStructure);

	/*
	 * Enable SPI_I2S_IT_RXNE interrupt
	 * SPI_I2S_IT_TXE interrupt will only be enabled when a write should happen
	 * */
	SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);

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
	/* Enable SPI_MASTER TXE interrupt */
	SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, ENABLE);
	/* Try to take the TX Semaphore */
	xSemaphoreTake(xTxSemaphore_2, portMAX_DELAY);

	/* Send byte through the SPIx peripheral */
	SPIx->DR = Data;

	/* Try to take the RX Semaphore */
	xSemaphoreTake(xRxSemaphore_2, portMAX_DELAY);

	/* Return the byte read from the SPI bus */
	return receivedByte_2;
}

/* Interrupt Handlers --------------------------------------------------------*/
#if SPI_NO == (2)
void SPI2_IRQHandler(void)
#elif SPI_NO == (1)
void SPI1_IRQHandler(void)
#endif
{
	static BaseType_t xHigherPriorityTaskWoken;

	/* Transmit buffer empty interrupt */
	if (SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_TXE) != RESET)
	{
		/* Release the semaphore */
		xSemaphoreGiveFromISR(xTxSemaphore_2, &xHigherPriorityTaskWoken);
		/* Disable SPI_MASTER TXE interrupt */
		SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
	}
	/* Receive buffer not empty interrupt */
	else if (SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_RXNE) != RESET)
	{
		/* Release the semaphore */
		xSemaphoreGiveFromISR(xRxSemaphore_2, &xHigherPriorityTaskWoken);
		/* Read the byte received in order to clear the interrupt flag */
		receivedByte_2 = SPI_I2S_ReceiveData(SPIx);
	}

	if (xHigherPriorityTaskWoken != pdFALSE)
	{
		/*
		 * If xHigherPriorityTaskWoken was set to true you we should yield.
		 * The actual macro used here is port specific.
		 */
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
