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
#include "hex_i2c.h"
//#include "hex_spi.h"

/* Defines -------------------------------------------------------------------*/
/* LIS331 Specifics */
#define LIS331_I2C_Init					HEX_I2C_Init
#define LIS331_I2C_BeginTransmission	HEX_I2C_BeginTransmission
#define LIS331_I2C_Write				HEX_I2C_Write
#define LIS331_I2C_EndTransmission		HEX_I2C_EndTransmission
#define LIS331_I2C_RequestFrom			HEX_I2C_RequestFrom

/* PCA9685 Specifics */
#define PCA9685_I2C_Init				HEX_I2C_Init
#define PCA9685_I2C_BeginTransmission	HEX_I2C_BeginTransmission
#define PCA9685_I2C_Write				HEX_I2C_Write
#define PCA9685_I2C_EndTransmission		HEX_I2C_EndTransmission
#define PCA9685_I2C_RequestFrom			HEX_I2C_RequestFrom
#define PCA9685_I2C_SlaveAtAddress		HEX_I2C_SlaveAtAddress

/* nRF24L01 Specifics */
/*
#define NRF24L01_SPI_Init		HEX_SPI_Init
#define NRF24L01_SPI_WriteRead	HEX_SPI_WriteRead
#define NRF24L01_SPI_Write		HEX_SPI_Write
*/

/* Circular Buffer Specifics */
#define CIRCULARBUFFER_SIZE			128
#define CIRCULARBUFFER_DATATYPE		volatile uint8_t

/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/

#endif /* BOARD_H_ */
