/**
 ******************************************************************************
 * @file	nrf24l01.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-05-04
 * @brief	
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef NRF24L01_H_
#define NRF24L01_H_

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "circularBuffer/circularBuffer.h"
#include "spi/spi.h"

/* Defines -------------------------------------------------------------------*/
#define MSB_BYTES(BYTES)	((BYTES >> 8) & 0xFFFFFFFF)
#define LSB_BYTE(BYTES)		(BYTES & 0xFF)

#define NRF24L01_MAX_AVAILABLE_DATA	CIRCULARBUFFER_SIZE

#define PAYLOAD_SIZE		32
#define DATA_COUNT_INDEX	0
#define MAX_DATA_COUNT		PAYLOAD_SIZE-1	// 1 byte datacount
#define PAYLOAD_FILLER_DATA	0xFF

/* Typedefs ------------------------------------------------------------------*/
typedef struct
{
	char* NRF24L01_DeviceName;

	uint16_t CSN_Pin;			/* Chip select pin */
	GPIO_TypeDef* CSN_GPIO;
	uint16_t CE_Pin;			/* Chip enable pin */
	GPIO_TypeDef* CE_GPIO;

	uint16_t IRQ_Pin;			/* Interrupt pin */
	GPIO_TypeDef* IRQ_GPIO;
	uint8_t IRQ_GPIO_PortSource;
	uint8_t IRQ_GPIO_PinSource;
	uint32_t IRQ_EXTI_Line;
	uint8_t IRQ_NVIC_IRQChannel;

	SPI_Device* SPIDevice;			/* SPI Device to use */

	CircularBuffer_TypeDef RxPipeBuffer[6];	/* Buffer for the six RX Pipes */

	SemaphoreHandle_t xTxSemaphore;				/* Semaphore for handling TX synchronization */
	SemaphoreHandle_t xDataAvailableSemaphore;	/* Semaphore for when data is available.
												 * Will be given when data is available on any pipe
												 */

	uint8_t RfChannel;		/* RF channel to use for the device, can be 0-125 */
	uint8_t* TxAddress;		/* TX address to use, the array set should be like uint8_t txAddress[5] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE}; */
	uint8_t* RxAddress0;	/* RX address to use for each pipe, the array should look like: */
	uint8_t* RxAddress1;	/* uint8_t rxAddress0[5] = {0x11, 0x22, 0x33, 0x44, 0x55}; */
	uint8_t* RxAddress2;
	uint8_t* RxAddress3;
	uint8_t* RxAddress4;
	uint8_t* RxAddress5;
	uint8_t* RxAddress6;
	uint8_t* RxAddress;

	uint8_t InTxMode;

} NRF24L01_Device;


/* Function prototypes -------------------------------------------------------*/
void NRF24L01_Init(NRF24L01_Device* Device);

void NRF24L01_WritePayload(NRF24L01_Device* Device, uint8_t* Data, uint8_t DataCount);

void NRF24L01_ReadRegister(NRF24L01_Device* Device, uint8_t Register, uint8_t* Buffer, uint8_t BufferSize);
void NRF24L01_WriteRegister(NRF24L01_Device* Device, uint8_t Register, uint8_t* Buffer, uint8_t BufferSize);

void NRF24L01_SetTxAddress(NRF24L01_Device* Device, uint8_t* Address);
void NRF24L01_SetRxAddressForPipe(NRF24L01_Device* Device, uint8_t* Address, uint8_t Pipe);
void NRF24L01_SetRFChannel(NRF24L01_Device* Device, uint8_t Channel);
void NRF24L01_SetPayloadSizeForPipe(NRF24L01_Device* Device, uint8_t Size, uint8_t Pipe);

void NRF24L01_EnablePipe(NRF24L01_Device* Device, uint8_t Pipe);

void NRF24L01_FlushTxBuffer(NRF24L01_Device* Device);
void NRF24L01_FlushRxBuffer(NRF24L01_Device* Device);
void NRF24L01_ResetAllFlags(NRF24L01_Device* Device);
void NRF24L01_ResetDataReadyFlag(NRF24L01_Device* Device);
void NRF24L01_ResetTxFlags(NRF24L01_Device* Device);

void NRF24L01_PowerUpInTxMode(NRF24L01_Device* Device);
void NRF24L01_PowerUpInRxMode(NRF24L01_Device* Device);
void NRF24L01_PowerDownMode(NRF24L01_Device* Device);

uint8_t NRF24L01_GetStatus(NRF24L01_Device* Device);
uint8_t NRF24L01_GetDataFromRxBuffer(NRF24L01_Device* Device, uint8_t* Buffer);
uint8_t NRF24L01_GetAvailableDataForPipe(NRF24L01_Device* Device, uint8_t Pipe);
void NRF24L01_GetDataFromPipe(NRF24L01_Device* Device, uint8_t Pipe, uint8_t* Buffer, uint8_t BufferSize);

void NRF24L01_PrintDebugInfo(NRF24L01_Device* Device);

void NRF24L01_Interrupt(NRF24L01_Device* Device);

#endif /* NRF24L01_H_ */
