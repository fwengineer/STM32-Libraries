/**
 ******************************************************************************
 * @file	hex_i2c.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-10
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "hex_i2c.h"

/* Private defines -----------------------------------------------------------*/
#define I2C_PERIPHERAL		I2C2
#define I2C_CLK				RCC_APB1Periph_I2C2
#define I2C_ER_IRQHandler	I2C2_ER_IRQHandler

#define I2C_GPIO			GPIOB
#define I2C_GPIO_CLK		RCC_APB2Periph_GPIOB
#define I2C_SCL_PIN			GPIO_Pin_10
#define I2C_SDA_PIN			GPIO_Pin_11

#define I2C_CLOCK_SPEED		200000

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the I2C
 * @param	None
 * @retval	None
 */
void HEX_I2C_Init()
{
	/* TODO: NOT WORKING!!! */

	/* Enable I2C Peripheral clock */
	RCC_APB1PeriphClockCmd(I2C_CLK, ENABLE);

	/* Enable GPIO and alternate function clock */
	RCC_APB2PeriphClockCmd(I2C_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);


	GPIO_InitTypeDef  GPIO_InitStructure;

    /* Configure I2C pins: SCL (I2C_SCL_PIN) and SDA (I2C_SDA_PIN) */
    GPIO_InitStructure.GPIO_Pin =  I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(I2C_GPIO, &GPIO_InitStructure);


	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_ClockSpeed = I2C_CLOCK_SPEED;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;

	/* I2C Peripheral Enable */
	I2C_Cmd(I2C_PERIPHERAL, ENABLE);
	/* Apply I2C configuration after enabling it */
	I2C_Init(I2C_PERIPHERAL, &I2C_InitStructure);

	/* Enable the error interrupt */
	I2C_ITConfig(I2C_PERIPHERAL, I2C_IT_ERR, ENABLE);
}


/**
 * @brief	Start transmission to a slave
 * @param	Address: Address to slave which transmission should be started with
 * @retval	...
 * 	- SUCCESS:
 * 	- ERROR:
 */
ErrorStatus HEX_I2C_BeginTransmission(uint8_t Address)
{
	/* Send START condition */
	I2C_GenerateSTART(I2C_PERIPHERAL, ENABLE);

	/* Make sure the START condition has been released on the bus */
	while (!I2C_CheckEvent(I2C_PERIPHERAL, I2C_EVENT_MASTER_MODE_SELECT));

	/* Send the address for write */
	I2C_Send7bitAddress(I2C_PERIPHERAL, Address << 1, I2C_Direction_Transmitter);

	/* Make sure a slave has acknowledge the address */
	while (!I2C_CheckEvent(I2C_PERIPHERAL, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	return SUCCESS;
}

/**
 * @brief	End transmission on the bus
 * @param	None
 * @retval	None
 */
void HEX_I2C_EndTransmission()
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
ErrorStatus HEX_I2C_Write(uint8_t Data)
{
	/* Send the byte to be written */
	I2C_SendData(I2C_PERIPHERAL, Data);

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
ErrorStatus HEX_I2C_RequestFrom(uint8_t Address, uint8_t* Storage, uint8_t NumByteToRead)
{
	// TODO: IS THIS NEEDED?
	/* While the bus is busy */
	while (I2C_GetFlagStatus(I2C_PERIPHERAL, I2C_FLAG_BUSY));

	/* Send START condition */
	I2C_GenerateSTART(I2C_PERIPHERAL, ENABLE);

	/* Make sure the START condition has been released on the bus */
	while (!I2C_CheckEvent(I2C_PERIPHERAL, I2C_EVENT_MASTER_MODE_SELECT));

	/* Send the address for read */
	I2C_Send7bitAddress(I2C_PERIPHERAL, Address << 1, I2C_Direction_Receiver);

	/* Make sure a slave has acknowledge the address */
	while (!I2C_CheckEvent(I2C_PERIPHERAL, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));


	/* While there is data to be read */
	while (NumByteToRead)
	{
		if (NumByteToRead == 0)
		{
			/* Send STOP Condition */
			I2C_GenerateSTOP(I2C_PERIPHERAL, ENABLE);
		}

		/* Check if data has been received */
		if (I2C_CheckEvent(I2C_PERIPHERAL, I2C_EVENT_MASTER_BYTE_RECEIVED))
		{
			/* Read a byte from the slave */
			*Storage = I2C_ReceiveData(I2C_PERIPHERAL);

			/* Point to the next location where the next byte read will be saved */
			Storage++;

			/* Decrement the read bytes counter */
			NumByteToRead--;
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
uint8_t HEX_I2C_SlaveAtAddress(const uint8_t Address)
{
	return SUCCESS;
}

/* Interrupt Handlers --------------------------------------------------------*/
/*
 * @brief	This function handles I2C errors
 */
void I2C_ER_IRQHandler(void)
{
	/* TODO: Do stuff when errors occur */
	/* Check flags */
	uint32_t test = 0;
}
