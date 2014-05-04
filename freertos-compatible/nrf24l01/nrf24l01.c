/**
 ******************************************************************************
 * @file	nrf24l01.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-05-04
 * @brief	
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "nrf24l01_register_map.h"
#include "nrf24l01.h"

/* Private defines -----------------------------------------------------------*/
#define CSN_LOW(DEVICE)				(Device->CSN_GPIO->BRR = Device->CSN_Pin)
#define CSN_HIGH(DEVICE)			(Device->CSN_GPIO->BSRR = Device->CSN_Pin)
#define SELECT_DEVICE(DEVICE)		(CSN_LOW(DEVICE))
#define DESELECT_DEVICE(DEVICE)		(CSN_HIGH(DEVICE))

#define CE_LOW(DEVICE)				(Device->CE_GPIO->BRR = Device->CE_Pin)
#define CE_HIGH(DEVICE)				(Device->CE_GPIO->BSRR = Device->CE_Pin)
#define DISABLE_DEVICE(DEVICE)		(CE_LOW(DEVICE))
#define ENABLE_DEVICE(DEVICE)		(CE_HIGH(DEVICE))

#define CONFIG_BASE					((1 << EN_CRC) | (0 << CRCO))

#define PIPE_FROM_STATUS(STATUS) 	(((STATUS) & 0xF) >> 1)

/* Private variables ---------------------------------------------------------*/
/* Private Function Prototypes -----------------------------------------------*/


/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the nRF24L01
 * @param	Device: The device to use
 * @retval	None
 */
void NRF24L01_Init(NRF24L01_Device* Device)
{
	/*
	 * Create the binary semaphores:
	 * The semaphore is created in the 'empty' state, meaning
	 * the semaphore must first be given before it can be taken (obtained)
	 * using the xSemaphoreTake() function.
	 */
	Device->xTxSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(Device->xTxSemaphore);


	/* Initialize RX Buffers */
	for (uint32_t i = 0; i < 6; i++)
	{
		CIRC_BUFFER_Init(&Device->RxPipeBuffer[i]);
	}


	/* Initialize GPIOs */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Initialize CE (Chip Enable) pin */
	if (Device->CE_GPIO == GPIOA)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	else if (Device->CE_GPIO == GPIOB)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin 		= Device->CE_Pin;
	GPIO_InitStructure.GPIO_Speed 		= GPIO_Speed_50MHz;
	GPIO_Init(Device->CE_GPIO, &GPIO_InitStructure);
	DISABLE_DEVICE(Device);

	/* Initialize CSN (Chip select) pin */
	if (Device->CSN_GPIO == GPIOA)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	else if (Device->CSN_GPIO == GPIOB)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin 		= Device->CSN_Pin;
	GPIO_InitStructure.GPIO_Speed 		= GPIO_Speed_50MHz;
	GPIO_Init(Device->CSN_GPIO, &GPIO_InitStructure);
	DESELECT_DEVICE(Device);

	/* Initialize IRQ (Interrupt) pin */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	if (Device->IRQ_GPIO == GPIOA)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	else if (Device->IRQ_GPIO == GPIOB)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin 		= Device->IRQ_Pin;
	GPIO_InitStructure.GPIO_Speed 		= GPIO_Speed_50MHz;
	GPIO_Init(Device->IRQ_GPIO, &GPIO_InitStructure);


	/* Initialize interrupt */
	/* Connect EXTIx Line to IRQ pin */
	GPIO_EXTILineConfig(Device->IRQ_GPIO_PortSource, Device->IRQ_GPIO_PinSource);

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line 		= Device->IRQ_EXTI_Line;
	EXTI_InitStructure.EXTI_LineCmd 	= ENABLE;
	EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger 	= EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable EXTIx Interrupt. The SPIx interrupt has to be higher than this so set EXTIx to the lowest priority*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel 						= Device->IRQ_NVIC_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= configLIBRARY_LOWEST_INTERRUPT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);


    /* Initialize spi peripheral */
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		/* 72 MHz/8 = 9 MHz, Datasheet page 53 says max 10 MHz (with Cload = 5 pF) */
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;				/* See datasheet page 50 */
	Device->SPIx_InitWithStructure(&SPI_InitStructure);

	Device->SPIx_WriteRead(0xAA);

	/* Set RF Channel */
	NRF24L01_SetRFChannel(Device, Device->RfChannel);

	/* Set payload size for the pipes, use same for all, and enable the pipes */
	for (uint32_t pipe = 0; pipe < 6; pipe++)
	{
		NRF24L01_SetPayloadSizeForPipe(Device, 32, pipe);
		NRF24L01_EnablePipe(Device, pipe);
	}

	/* Set the TX address */
	NRF24L01_SetTxAddress(Device, Device->TxAddress);

	/* Set RX address for all pipes */
	NRF24L01_SetRxAddressForPipe(Device, Device->RxAddress0, 0);
	NRF24L01_SetRxAddressForPipe(Device, Device->RxAddress1, 1);
	NRF24L01_SetRxAddressForPipe(Device, Device->RxAddress2, 2);
	NRF24L01_SetRxAddressForPipe(Device, Device->RxAddress3, 3);
	NRF24L01_SetRxAddressForPipe(Device, Device->RxAddress4, 4);
	NRF24L01_SetRxAddressForPipe(Device, Device->RxAddress5, 5);

	/* Flush the buffers to get rid of old data and reset the flags in the STATUS register */
	NRF24L01_FlushTxBuffer(Device);
	NRF24L01_FlushRxBuffer(Device);
	NRF24L01_ResetAllFlags(Device);

	/* Power up the device i RX mode to start listening for packets */
	NRF24L01_PowerUpInRxMode(Device);
}

/**
 * @brief	Write data and send it to the address specified in TX_ADDR
 * @param	Device: The device to use
 * @param	Data: Pointer to where the data is stored
 * @param	ByteCount: The number of bytes in Data
 * @retval	None
 */
void NRF24L01_WritePayload(NRF24L01_Device* Device, uint8_t* Data, uint8_t DataCount)
{
	/* You can only send the amount of data specified in MAX_DATA_COUNT */
	if (DataCount > MAX_DATA_COUNT) return;

	/* Try to take the semaphore */
	xSemaphoreTake(Device->xTxSemaphore, portMAX_DELAY);

	/* Semaphore was taken so we can proceed with sending new data */
	DISABLE_DEVICE(Device);				/* Disable the device while sending data to TX buffer */
	NRF24L01_PowerUpInTxMode(Device);	/* Power up in TX mode */
	NRF24L01_FlushTxBuffer(Device);		/* Flush the TX buffer */

	SELECT_DEVICE(Device);

	Device->SPIx_WriteRead(W_TX_PAYLOAD);	/* We want to write the TX payload */
	Device->SPIx_WriteRead(DataCount);		/* Write the data count */
	uint32_t i;
	for (i = 0; i < DataCount; i++)
	{
		Device->SPIx_WriteRead(Data[i]);	/* Write the data */
	}
	for (i++; i <= MAX_DATA_COUNT; i++)
	{
		Device->SPIx_WriteRead(PAYLOAD_FILLER_DATA);	/* Fill the rest of the payload with filler data */
	}

	DESELECT_DEVICE(Device);
    ENABLE_DEVICE(Device);
}

/**
 * @brief
 * @param	Device: The device to use
 * @retval	None
 */
void NRF24L01_ReadRegister(NRF24L01_Device* Device, uint8_t Register, uint8_t* Buffer, uint8_t BufferSize)
{
	SELECT_DEVICE(Device);
	uint8_t status = Device->SPIx_WriteRead(R_REGISTER | Register);

	/* R_REGISTER command only have 5 data bytes, datasheet page 51 */
	if (BufferSize > 5)
		BufferSize = 5;

	/* LSByte first, datasheet page 50 */
	for (int32_t i = BufferSize-1; i >= 0; i--)
	{
		Buffer[i] = Device->SPIx_WriteRead(NOP);
	}
	DESELECT_DEVICE(Device);
}

/**
 * @brief
 * @param	Device: The device to use
 * @retval	None
 */
void NRF24L01_WriteRegister(NRF24L01_Device* Device, uint8_t Register, uint8_t* Buffer, uint8_t BufferSize)
{
	DISABLE_DEVICE(Device);	 /* W_REGISTER is executable in power down or standby modes only */
	SELECT_DEVICE(Device);
	uint8_t status = Device->SPIx_WriteRead(W_REGISTER | Register);

	/* W_REGISTER command only have 5 data bytes, datasheet page 51 */
	if (BufferSize > 5)
		BufferSize = 5;

	/* LSByte first, datasheet page 50 */
	for (int32_t i = BufferSize-1; i >= 0; i--)
	{
		/* Have to do WriteRead for some reason, otherwise one byte will be lost */
		status = Device->SPIx_WriteRead(Buffer[i]);
	}
	DESELECT_DEVICE(Device);
	ENABLE_DEVICE(Device);
}

/**
 * @brief
 * @param	Device: The device to use
 * @param	Address: A 5-byte array with the address to set [MSByte ... LSByte]
 * @retval	None
 */
void NRF24L01_SetTxAddress(NRF24L01_Device* Device, uint8_t* Address)
{
	NRF24L01_WriteRegister(Device, TX_ADDR, Address, 5);
}

/**
 * @brief
 * @param	Device: The device to use
 * @param	Address: A 5-byte array with the address to set [MSByte ... LSByte]
 * @param	Pipe: The pipe to set the address for. Can be 0 to 5. Pipe 0-1 are 5 bytes, pipe 2-5 are 1 byte
 * @retval	None
 */
void NRF24L01_SetRxAddressForPipe(NRF24L01_Device* Device, uint8_t* Address, uint8_t Pipe)
{
	if (Pipe < 2)
		NRF24L01_WriteRegister(Device, RX_ADDR_P0 + Pipe, Address, 5);
	else if (Pipe < 6)
		NRF24L01_WriteRegister(Device, RX_ADDR_P0 + Pipe, Address, 1);
}

/**
 * @brief	Set the RF channel to transmit on
 * @param	Device: The device to use
 * @param	Channel: The channel
 * @retval	None
 * @note	Freq = 2400 + RF_CH [MHz], -> 2400 MHz - 2525 MHz operation frequencies
 */
void NRF24L01_SetRFChannel(NRF24L01_Device* Device, uint8_t Channel)
{
	if (Channel <= 125)
	{
		NRF24L01_WriteRegister(Device, W_REGISTER | RF_CH, &Channel, 1);
	}
}

/**
 * @brief	Set the size of the payload for a specified pipe
 * @param	Device: The device to use
 * @param	Size: The size of the payload, can be 0 to 32 bytes, where 0 means pipe is not used
 * @param	Pipe: The pipe to set, can be 0 to 5
 * @retval	None
 */
void NRF24L01_SetPayloadSizeForPipe(NRF24L01_Device* Device, uint8_t Size, uint8_t Pipe)
{
	if (Pipe < 6 && Size < 33)
	{
		NRF24L01_WriteRegister(Device, RX_PW_P0 + Pipe, &Size, 1);
	}
}

/**
 * @brief	Enable a pipe
 * @param	Device: The device to use
 * @param	Pipe: The pipe to enable, can be 0 to 5
 * @retval	None
 */
void NRF24L01_EnablePipe(NRF24L01_Device* Device, uint8_t Pipe)
{
	if (Pipe < 6)
	{
		uint8_t value = 0;
		NRF24L01_ReadRegister(Device, EN_RXADDR, &value, 1);	/* Get the old value */
		value |= (1 << Pipe);											/* Enable the new pipe */
		NRF24L01_WriteRegister(Device, EN_RXADDR, &value, 1);	/* Set the new value */
	}
}

/**
 * @brief	Flush the TX Buffer
 * @param	Device: The device to use
 * @retval	None
 */
void NRF24L01_FlushTxBuffer(NRF24L01_Device* Device)
{
	SELECT_DEVICE(Device);
	Device->SPIx_WriteRead(FLUSH_TX);
	DESELECT_DEVICE(Device);
}

/**
 * @brief	Flush the RX Buffer
 * @param	Device: The device to use
 * @retval	None
 */
void NRF24L01_FlushRxBuffer(NRF24L01_Device* Device)
{
	SELECT_DEVICE(Device);
	Device->SPIx_WriteRead(FLUSH_RX);
	DESELECT_DEVICE(Device);
}

/**
 * @brief	Reset all flags in the STATUS register
 * @param	Device: The device to use
 * @retval	None
 */
void NRF24L01_ResetAllFlags(NRF24L01_Device* Device)
{
	uint8_t data = (1 << TX_DS) | (1 << MAX_RT) | (1 << RX_DR);
	NRF24L01_WriteRegister(Device, STATUS, &data, 1);
}

/**
 * @brief	Reset the Data Ready (RX_DR) flag in the STATUS register
 * @param	Device: The device to use
 * @retval	None
 */
void NRF24L01_ResetDataReadyFlag(NRF24L01_Device* Device)
{
	uint8_t data = (1 << RX_DR);
	NRF24L01_WriteRegister(Device, STATUS, &data, 1);
}

/**
 * @brief	Reset flags related to TX (TX_DS, MAX_RT) in the STATUS register
 * @param	Device: The device to use
 * @retval	None
 */
void NRF24L01_ResetTxFlags(NRF24L01_Device* Device)
{
	uint8_t data = (1 << TX_DS) | (1 << MAX_RT);
	NRF24L01_WriteRegister(Device, STATUS, &data, 1);
}

/**
 * @brief	Power up the device in TX mode
 * @param	Device: The device to use
 * @retval	None
 */
void NRF24L01_PowerUpInTxMode(NRF24L01_Device* Device)
{
	uint8_t data = CONFIG_BASE | (1 << PWR_UP) | (0 << PRIM_RX);
	NRF24L01_WriteRegister(Device, CONFIG, &data, 1);
}

/**
 * @brief	Power up the device in RX mode
 * @param	Device: The device to use
 * @retval	None
 */
void NRF24L01_PowerUpInRxMode(NRF24L01_Device* Device)
{
	uint8_t data = CONFIG_BASE | (1 << PWR_UP) | (1 << PRIM_RX);
	NRF24L01_WriteRegister(Device, CONFIG, &data, 1);
}

/**
 * @brief	Power down the device
 * @param	Device: The device to use
 * @retval	None
 */
void NRF24L01_PowerDownMode(NRF24L01_Device* Device)
{
	uint8_t data = CONFIG_BASE | (0 << PWR_UP);
	NRF24L01_WriteRegister(Device, CONFIG, &data, 1);
}

/**
 * @brief	Reads the STATUS register in the nRF24L01
 * @param	Device: The device to use
 * @retval	The status register
 */
uint8_t NRF24L01_GetStatus(NRF24L01_Device* Device)
{
	SELECT_DEVICE(Device);
	uint8_t status = Device->SPIx_WriteRead(NOP);
	DESELECT_DEVICE(Device);

	return status;
}

/**
 * @brief	Get data from the RX buffer
 * @param	Device: The device to use
 * @param	Storage: Pointer to where the data should be stored
 * @retval	The amount of data received
 */
uint8_t NRF24L01_GetDataFromRxBuffer(NRF24L01_Device* Device, uint8_t* Buffer)
{
	SELECT_DEVICE(Device);
	Device->SPIx_WriteRead(R_RX_PAYLOAD);
	uint32_t dataCount = Device->SPIx_WriteRead(NOP);

	for (uint32_t i = 0; i < dataCount + 1; i++)
	{
		Buffer[i] = Device->SPIx_WriteRead(NOP);
	}
	DESELECT_DEVICE(Device);

//	NRF24L01_FlushRxBuffer(Device);	/* TODO: Is this needed??? */

	return dataCount;
}

/**
 * @brief	Check if a pipe has available data in it's buffer
 * @param	Device: The device to use
 * @param	Pipe: The pipe to check for data
 * @retval	The available data for the specified pipe
 */
uint8_t NRF24L01_GetAvailableDataForPipe(NRF24L01_Device* Device, uint8_t Pipe)
{
	if (Pipe < 6)
	{
		return CIRC_BUFFER_GetCount(&Device->RxPipeBuffer[Pipe]);
	}
	return 0;
}

/**
 * @brief	Get data from a specified pipe
 * @param	Device: The device to use
 * @param	Pipe: The pipe to check for data
 * @param	Storage: Pointer to where the data should be stored
 * @param	DataCount: The amount of data to get
 * @retval	None
 * @note	It's good to check that the requested amount of data is available
			first by calling NRF24L01_GetAvailableDataForPipe()
 */
void NRF24L01_GetDataFromPipe(NRF24L01_Device* Device, uint8_t Pipe, uint8_t* Buffer, uint8_t BufferSize)
{
	if (Pipe < 6)
	{
		uint8_t i;
		for (i = 0; i < BufferSize; i++)
		{
			Buffer[i] = CIRC_BUFFER_Remove(&Device->RxPipeBuffer[Pipe]);
		}
	}
}

/* Debug Print ---------------------------------------------------------------*/
#ifdef DEBUG
/**
 * @brief	Write some debug info to the UART
 * @param	Device: The device to use
 * @param	None
 * @retval	None
 */
void NRF24L01_PrintDebugInfo(NRF24L01_Device* Device)
{
	printf("------- nRF24L01 Debug Info -------\n");
	printf("Name: %s\n", Device->NRF24L01_DeviceName);

	/* ============================ */
	uint8_t config;
	NRF24L01_ReadRegister(Device, CONFIG, &config, 1);
	printf("CONFIG: 0x%02x\n", config);

	/* ============================ */
	uint8_t en_aa;
	NRF24L01_ReadRegister(Device, EN_AA, &en_aa, 1);
	printf("EN_AA: 0x%02x\n", en_aa);

	/* ============================ */
	uint8_t en_rxaddr;
	NRF24L01_ReadRegister(Device, EN_RXADDR, &en_rxaddr, 1);
	printf("EN_RXADDR: 0x%02x\n", en_rxaddr);

	/* ============================ */
	uint8_t setupAw;
	NRF24L01_ReadRegister(Device, SETUP_AW, &setupAw, 1);
	printf("SETUP_AW: 0x%02x\n", setupAw);

	/* ============================ */
	uint8_t setupRetr;
	NRF24L01_ReadRegister(Device, SETUP_RETR, &setupRetr, 1);
	printf("SETUP_RETR: 0x%02x\n", setupRetr);

	/* ============================ */
	uint8_t rfChannel;
	NRF24L01_ReadRegister(Device, RF_CH, &rfChannel, 1);
	printf("RF_CH: 0x%02x\n", rfChannel);

	/* ============================ */
	uint8_t rfSetup;
	NRF24L01_ReadRegister(Device, RF_SETUP, &rfSetup, 1);
	printf("RF_SETUP: 0x%02x\n", rfSetup);

	/* ============================ */
	uint8_t status = NRF24L01_GetStatus(Device);
	printf("STATUS: 0x%02x\n", status);

	/* ============================ */
//	uint8_t pipe = NRF24L01_GetPipeNumber(Device);
//	if (0x07 == pipe)
//		printf("Pipe: RF FIFO Empty\n");
//	else if (0x06 == pipe)
//		printf("Pipe: ERROR - Should not happen!\n");
//	else
//		printf("Pipe: %d\n", pipe);

	/* ============================ */
	uint8_t observeTx;
	NRF24L01_ReadRegister(Device, OBSERVE_TX, &observeTx, 1);
	printf("OBSERVE_TX: 0x%02x\n", observeTx);

	/* ============================ */
	uint8_t rpd;
	NRF24L01_ReadRegister(Device, RPD, &rpd, 1);
	printf("RPD: 0x%02x\n", rpd);

	/* ============================ */
	uint8_t addr0[5];
	NRF24L01_ReadRegister(Device, RX_ADDR_P0, addr0, 5);
	printf("RX_ADDR_P0: 0x%02x%02x%02x%02x%02x\n", addr0[4], addr0[3], addr0[2], addr0[1], addr0[0]);

	uint8_t addr1[5];
	NRF24L01_ReadRegister(Device, RX_ADDR_P1, addr1, 5);
	printf("RX_ADDR_P1: 0x%02x%02x%02x%02x%02x\n", addr1[4], addr1[3], addr1[2], addr1[1], addr1[0]);

	uint8_t addr2;
	NRF24L01_ReadRegister(Device, RX_ADDR_P2, &addr2, 1);
	printf("RX_ADDR_P2: 0x%02x\n", addr2);

	uint8_t addr3;
	NRF24L01_ReadRegister(Device, RX_ADDR_P3, &addr3, 1);
	printf("RX_ADDR_P3: 0x%02x\n", addr3);

	uint8_t addr4;
	NRF24L01_ReadRegister(Device, RX_ADDR_P4, &addr4, 1);
	printf("RX_ADDR_P4: 0x%02x\n", addr4);

	uint8_t addr5;
	NRF24L01_ReadRegister(Device, RX_ADDR_P5, &addr5, 1);
	printf("RX_ADDR_P5: 0x%02x\n", addr5);

	/* ============================ */
	uint8_t addrTx[5];
	NRF24L01_ReadRegister(Device, TX_ADDR, addrTx, 5);
	printf("TX_ADDR: 0x%02x%02x%02x%02x%02x\n", addrTx[4], addrTx[3], addrTx[2], addrTx[1], addrTx[0]);

	/* ============================ */
	uint8_t payloads[6];
	NRF24L01_ReadRegister(Device, RX_PW_P0, payloads, 6);
	printf("RX_PW_P0-5: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n", payloads[0], payloads[1], payloads[2], payloads[3], payloads[4], payloads[5]);

	/* ============================ */
	uint8_t fifoStatus;
	NRF24L01_ReadRegister(Device, FIFO_STATUS, &fifoStatus, 1);
	printf("FIFO_STATUS: 0x%02x\n", fifoStatus);

	/* ============================ */
	uint8_t dynpd;
	NRF24L01_ReadRegister(Device, DYNPD, &dynpd, 1);
	printf("DYNPD: 0x%02x\n", dynpd);

	printf("-----------------------------------\n");
}
#endif

/* Interrupt Service Routines ------------------------------------------------*/
void NRF24L01_Interrupt(NRF24L01_Device* Device)
{
	static BaseType_t xHigherPriorityTaskWoken;

	uint8_t status = NRF24L01_GetStatus(Device);
	/* Data Sent TX FIFO interrupt, asserted when packet transmitted on TX */
	if (status & (1 << TX_DS))
	{
		NRF24L01_PowerUpInRxMode(Device);
		NRF24L01_ResetTxFlags(Device);
		/* Give the semaphore because we are done transmitting */
		xSemaphoreGiveFromISR(Device->xTxSemaphore, &xHigherPriorityTaskWoken);
	}
	/* Maximum number of TX retransmits interrupt */
	else if (status & (1 << MAX_RT))
	{
		NRF24L01_PowerUpInRxMode(Device);
		NRF24L01_ResetTxFlags(Device);
		/* Give the semaphore because we are done transmitting */
		xSemaphoreGiveFromISR(Device->xTxSemaphore, &xHigherPriorityTaskWoken);
	}
	/* Data Ready interrupt */
	else if (status & (1 << RX_DR))
	{
		uint8_t pipe = PIPE_FROM_STATUS(status);
		if (pipe < 6)
		{
			uint8_t buffer[32];
			uint8_t availableData = NRF24L01_GetDataFromRxBuffer(Device, buffer);

			for (uint32_t i = 0; i < availableData; i++)
			{
				if (!CIRC_BUFFER_IsFull(&Device->RxPipeBuffer[pipe]))
					CIRC_BUFFER_Insert(&Device->RxPipeBuffer[pipe], buffer[i]);
			}
		}
		NRF24L01_ResetDataReadyFlag(Device);
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
