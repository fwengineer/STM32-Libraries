/**
 ******************************************************************************
 * @file	i2c1_remap.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-02-18
 * @brief	Manage I2C1
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "i2c1_remap.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* Private defines -----------------------------------------------------------*/
#define I2C_PERIPHERAL		I2C1

#define I2C_GPIO			GPIOB
#define I2C_SCL_PIN			GPIO_Pin_8
#define I2C_SDA_PIN			GPIO_Pin_9

#define I2C_CLOCK_SPEED		400000

#define RTOS_QUICK_FIX_DELAY	(1 / portTICK_PERIOD_MS)

/* Private variables ---------------------------------------------------------*/
SemaphoreHandle_t xSemStartCondition = NULL;

/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the I2C
 * @param	None
 * @retval	None
 */
void I2C1_Init()
{
	/* Semaphores */
	//xSemStartCondition = xSemaphoreCreateBinary();

	/* Remap I2C1 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);

	/* I2C clock & GPIOB enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* I2C SDA and SCL configuration */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(I2C_GPIO, &GPIO_InitStructure);

//	/* NVIC Configuration */
//	NVIC_InitTypeDef NVIC_InitStructure;
//	/* Event interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	/* Error interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

	/* I2C Configuration */
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_ClockSpeed = I2C_CLOCK_SPEED;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_Init(I2C_PERIPHERAL, &I2C_InitStructure);

	/* Enable I2C*/
	I2C_Cmd(I2C_PERIPHERAL, ENABLE);

	/* Enable the I2C1 interrupts */
//	I2C_ITConfig(I2C_PERIPHERAL, I2C_IT_EVT, ENABLE);
//	I2C_ITConfig(I2C_PERIPHERAL, I2C_IT_ERR, ENABLE);
}


/**
 * @brief	Start transmission to a slave
 * @param	Address: Address to slave which transmission should be started with
 * @retval	...
 * 	- SUCCESS:
 * 	- ERROR:
 */
ErrorStatus I2C1_BeginTransmission(uint8_t Address)
{
	/* Send START condition */
	I2C_GenerateSTART(I2C_PERIPHERAL, ENABLE);

	/* TODO: Replace with RTOS stuff */
	/* Make sure the START condition has been released on the bus */
	while (!I2C_CheckEvent(I2C_PERIPHERAL, I2C_EVENT_MASTER_MODE_SELECT));

//	/* Try to take the semaphore */
//	if (!xSemaphoreTake(xSemStartCondition, portMAX_DELAY))
//	{
//		while (1);
//	}

	/* Send the address for write */
	I2C_Send7bitAddress(I2C_PERIPHERAL, Address << 1, I2C_Direction_Transmitter);

	/* TODO: Replace with RTOS stuff */
	/* Make sure a slave has acknowledge the address */
	while (!I2C_CheckEvent(I2C_PERIPHERAL, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	return SUCCESS;
}

/**
 * @brief	End transmission on the bus
 * @param	None
 * @retval	None
 */
void I2C1_EndTransmission()
{
	I2C_GenerateSTOP(I2C_PERIPHERAL, ENABLE);
}

/**
 * @brief	Write real data to the bus
 * @param	Data: The data to write
 * @retval	...
 * 	- SUCCESS:
 * 	- ERROR:
 */
ErrorStatus I2C1_Write(uint8_t Data)
{
	/* Send the byte to be written */
	I2C_SendData(I2C_PERIPHERAL, Data);

	/* TODO: Replace with RTOS stuff,
	 * see http://www.freertos.org/FreeRTOS_Support_Forum_Archive/September_2013/freertos_Best_way_to_handle_While_on_buses_8688418.html */
	/* Make sure data has been transmitted */
	while (!I2C_CheckEvent(I2C_PERIPHERAL, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	return SUCCESS;
}

/**
 * @brief	Requests a given amount of data from a slave
 * @param	Address: The address to the slave to request from
 * @param	Storage: Pointer to where the data should be stored
 * @param	ByteCount: The amount of bytes to receive
 * @retval	...
 * 	- SUCCESS:
 * 	- ERROR:
 */
ErrorStatus I2C1_RequestFrom(uint8_t Address, uint8_t* Storage, uint16_t NumByteToRead)
{
	/* TODO: Replace with RTOS stuff */
	/* While the bus is busy */
	while (I2C_GetFlagStatus(I2C_PERIPHERAL, I2C_FLAG_BUSY));

	/* Send START condition */
	I2C_GenerateSTART(I2C_PERIPHERAL, ENABLE);

	/* TODO: Replace with RTOS stuff */
	/* Make sure the START condition has been released on the bus */
	while (!I2C_CheckEvent(I2C_PERIPHERAL, I2C_EVENT_MASTER_MODE_SELECT));

	/* Send the address for read */
	I2C_Send7bitAddress(I2C_PERIPHERAL, Address << 1, I2C_Direction_Receiver);

	/* TODO: Replace with RTOS stuff */
	/* Make sure a slave has acknowledge the address */
	while (!I2C_CheckEvent(I2C_PERIPHERAL, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));


	/* While there is data to be read */
	while (NumByteToRead)
	{
		/* Check if data has been received */
		if (I2C_CheckEvent(I2C_PERIPHERAL, I2C_EVENT_MASTER_BYTE_RECEIVED))
		{
			/* Read a byte from the slave */
			*Storage = I2C_ReceiveData(I2C_PERIPHERAL);

			/* Point to the next location where the next byte read will be saved */
			Storage++;

			/* Decrement the read bytes counter */
			NumByteToRead--;
			if (NumByteToRead == 0)
			{
				/* Send STOP Condition */
				I2C_GenerateSTOP(I2C_PERIPHERAL, ENABLE);
			}
		}
	}

	return SUCCESS;
}

/**
 * @brief	Checks to see if there is a slave at a given address. This is done by beginning a transmission
 *			and see if a slave is responding with an ACK or not
 * @param	Address: The address to look for a slave at
 * @retval	1: There is a slave at the address [Address]
 * @retval	0: There is no slave there
 */
uint8_t I2C1_SlaveAtAddress(const uint8_t Address)
{
	/* TODO: Implement? */
	return SUCCESS;
}

/* Interrupt Handlers --------------------------------------------------------*/
/*
 * @brief	This IRQ handler handles I2C errors
 */
//void I2C1_ER_IRQHandler(void)
//{
//	/* TODO: Replace with RTOS stuff */
//	/* Check flags */
//}

/*
 * @brief	This IRQ handler handles I2C events
 */
//void I2C1_EV_IRQHandler(void)
//{
//	static signed portBASE_TYPE xHigherPriorityTaskWoken;
//	xHigherPriorityTaskWoken = pdFALSE;
//
//	if (I2C_CheckEvent(I2C_PERIPHERAL, I2C_EVENT_MASTER_MODE_SELECT))
//	{
//		xHigherPriorityTaskWoken = 1;
//		//xSemaphoreGiveFromISR(xSemStartCondition, &xHigherPriorityTaskWoken);
//	}
//
//	/*
//	switch (I2C_GetLastEvent(I2C1))
//	{
//		case I2C_EVENT_MASTER_MODE_SELECT:
//
//			break;
//		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
//			//
//			break;
//		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
//			//
//			break;
//		default:
//			break;
//	}
//	*/
//
//	 /* If xHigherPriorityTaskWoken was set to true you
//	    we should yield.  The actual macro used here is
//	    port specific. */
//	//portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
//}
