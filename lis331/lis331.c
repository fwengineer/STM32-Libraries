/**
 ******************************************************************************
 * @file	lis331.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-04-04
 * @brief	Contains functions to manage the LIS331 Accelerometer
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "lis331.h"

/* Private defines -----------------------------------------------------------*/
#define WHO_AM_I			0x0F
#define LIS331_DEVICE_ID	0x32

#define CTRL_REG1			0x20
#define XEN					0
#define YEN					1
#define ZEN					2
#define DR0					3
#define DR1					4
#define PM0					5
#define PM1					6
#define PM2					7

#define CTRL_REG2			0x21
#define HPCF0				0
#define HPCF1				1
#define HPEN1				2
#define HPEN2				3
#define FDS					4
#define HPM0				5
#define HPM1				6
#define BOOT				7

#define CTRL_REG3			0x22
#define I1_CFG0				0
#define I1_CFG1				1
#define LIR1				2
#define I2_CFG0				3
#define I2_CFG1				4
#define LIR2				5
#define PP_OD				6
#define IHL					7

#define CTRL_REG4			0x23
#define SIM					0
#define ST					1
#define STSIGN				3
#define FS0					4
#define FS1					5
#define BLE					6
#define BDU					7

#define CTRL_REG5			0x24
#define TURNON0				0
#define TURNON1				1

#define HP_FILTER_RESET		0x25
#define REFERENCE			0x26

#define STATUS_REG			0x27
#define XDA					0
#define YDA					1
#define ZDA					2
#define ZYXDA				3
#define XOR					4
#define YOR					5
#define ZOR					6
#define ZYXOR				7

#define OUT_X_L				0x28
#define OUT_X_H				0x29
#define OUT_Y_L				0x2A
#define OUT_Y_H				0x2B
#define OUT_Z_L				0x2C
#define OUT_Z_H				0x2D

/* Interrupts */
#define INT1_CFG			0x30
#define XLIE				0
#define XHIE				1
#define YLIE				2
#define YHIE				3
#define ZLIE				4
#define ZHIE				5
#define SIXDIR				6
#define AOI					7

#define INT1_SOURCE			0x31
#define LIS331_XL			0
#define LIS331_XH			1
#define LIS331_YL			2
#define LIS331_YH			3
#define LIS331_ZL			4
#define LIS331_ZH			5
#define LIS331_IA			6

#define INT1_THS			0x32
#define INT1_DURATION		0x33

#define INT2_CFG			0x34
#define INT2_SOURCE			0x35
#define INT2_THS			0x36
#define INT2_DURATION		0x37

#define AUTO_INCREMENT(START)	((START) | 0x80)

/* Private variables ---------------------------------------------------------*/
uint8_t _lis331Address;

/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the LIS331
 * @param	None
 * @retval	None
 */
uint8_t LIS331_Init(uint8_t Address)
{
	LIS331_I2C_Init();
	
	_lis331Address = Address;
	
	LIS331_I2C_BeginTransmission(_lis331Address);
	LIS331_I2C_Write(WHO_AM_I);
	LIS331_I2C_EndTransmission();
	
	uint8_t deviceId;
	LIS331_I2C_RequestFrom(_lis331Address, &deviceId, 1);
	if (deviceId != LIS331_DEVICE_ID)
	{
		return 0;
	}
	else
	{
		//Normal power mode, 1000 Hz data rate, enable X,
		uint8_t ctrlReg1 = (1 << PM0) | (1 << DR1) | (1 << DR0) | (1 << ZEN) | (1 << YEN) | (1 << XEN);
		LIS331_I2C_BeginTransmission(_lis331Address);
		LIS331_I2C_Write(CTRL_REG1);
		LIS331_I2C_Write(ctrlReg1);
		LIS331_I2C_EndTransmission();
		return 1;
	}		
}

void LIS331_GetXYZData(int16_t *X, int16_t *Y, int16_t *Z)
{	
	LIS331_I2C_BeginTransmission(_lis331Address);
	LIS331_I2C_Write(AUTO_INCREMENT(OUT_X_L));
	LIS331_I2C_EndTransmission();
	
	uint8_t data[6] = {};
	LIS331_I2C_RequestFrom(_lis331Address, data, 6);
	
	*X = (data[1] << 8) | data[0];
	*Y = (data[3] << 8) | data[2];
	*Z = (data[5] << 8) | data[4];
}

void LIS331_EnableSelfTest()
{
	LIS331_I2C_BeginTransmission(_lis331Address);
	LIS331_I2C_Write(CTRL_REG4);
	LIS331_I2C_EndTransmission();
	
	uint8_t ctrlReg4;
	LIS331_I2C_RequestFrom(_lis331Address, &ctrlReg4, 1);
	
	ctrlReg4 |= (1 << ST);

	LIS331_I2C_BeginTransmission(_lis331Address);
	LIS331_I2C_Write(CTRL_REG4);
	LIS331_I2C_Write(ctrlReg4);
	LIS331_I2C_EndTransmission();
}

void LIS331_DisableSelfTest()
{
	LIS331_I2C_BeginTransmission(_lis331Address);
	LIS331_I2C_Write(CTRL_REG4);
	LIS331_I2C_EndTransmission();
	
	uint8_t ctrlReg4;
	LIS331_I2C_RequestFrom(_lis331Address, &ctrlReg4, 1);
	
	ctrlReg4 &= ~(1 << ST);

	LIS331_I2C_BeginTransmission(_lis331Address);
	LIS331_I2C_Write(CTRL_REG4);
	LIS331_I2C_Write(ctrlReg4);
	LIS331_I2C_EndTransmission();
}

/* Interrupt Service Routines ------------------------------------------------*/
