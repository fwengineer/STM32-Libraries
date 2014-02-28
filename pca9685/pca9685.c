/**
 ******************************************************************************
 * @file	pca9685.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-25
 * @brief	Contains functions to manage the PCA9685 LED Driver
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "pca9685.h"

/* Private defines -----------------------------------------------------------*/
#define MODE1				0x00
	#define MODE1_ALLCALL	0
	#define MODE1_SUB3		1
	#define MODE1_SUB2		2
	#define MODE1_SUB1		3
	#define MODE1_SLEEP		4
	#define MODE1_AI		5
	#define MODE1_EXTCLK	6
	#define MODE1_RESTART	7

#define MODE2				0x01
	#define MODE2_OUTNE0	0
	#define MODE2_OUTNE1	1
	#define MODE2_OUTDRV	2
	#define MODE2_OCH		3
	#define MODE2_INVRT		4

#define SUBADR1				0x02
#define SUBADR2				0x03
#define SUBADR3				0x04
#define ALLCALLADR			0x05

#define LEDn_ON_L(n)		(0x06 + (n)*4)
#define LEDn_ON_H(n)		(0x07 + (n)*4)
#define LEDn_OFF_L(n)		(0x08 + (n)*4)
#define LEDn_OFF_H(n)		(0x09 + (n)*4)

#define ALL_LED_ON_L		0xFA
#define ALL_LED_ON_H		0xFB
#define ALL_LED_OFF_L		0xFC
#define ALL_LED_OFF_H		0xFD

#define PRE_SCALE			0xFE

#define MAX_OUTPUT_INDEX	15
#define MAX_OUTPUT_VALUE	0xFFF

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
 * @brief	Initializes the PCA9685
 * @param	None
 * @retval	1: A PCA9685 at address [Address] has been initialized
 * @retval	0: Initialization failed
 */
uint8_t PCA9685_Init(PCA9685_Init_TypeDef *PCA9685_InitStruct)
{
	// Check parameters
	assert_param(IS_PCA9685_INV_OUTPUTS(PCA9685_InitStruct->InvOutputs));
    assert_param(IS_PCA9685_OUTPUT_DRIVER(PCA9685_InitStruct->OutputDriver));
    assert_param(IS_PCA9685_OUTPUT_NOT_EN(PCA9685_InitStruct->OutputNotEn));
	assert_param(IS_PCA9685_FREQUENCY(PCA9685_InitStruct->PWMFrequency));

	PCA9685_I2C_Init();
	
	uint8_t twiStatus = 0;
	
	if (PCA9685_I2C_SlaveAtAddress(PCA9685_InitStruct->Address))
	{
		// Sleep function does not work for some reason
		//PCA9685_Sleep(PCA9685_InitStruct->Address);
		PCA9685_I2C_BeginTransmission(PCA9685_InitStruct->Address);
		PCA9685_I2C_Write(MODE1);
		PCA9685_I2C_Write(1 << MODE1_SLEEP);
		PCA9685_I2C_EndTransmission();

		/* PRE_SCALE Register:
		 * Set to value specified in PCA9685_InitStruct->PWMFrequency;
		 * Has to be set when device is in sleep mode
		 */
		PCA9685_I2C_BeginTransmission(PCA9685_InitStruct->Address);
		PCA9685_I2C_Write(PRE_SCALE);
		PCA9685_I2C_Write(PCA9685_InitStruct->PWMFrequency);
		PCA9685_I2C_EndTransmission();

		/* MODE1 Register:
		 * Internal clock
		 * Register Auto-Increment enabled
		 * Normal mode
		 * Does not respond to subaddresses
		 * Responds to All Call I2C-bus address
		 */
		PCA9685_I2C_BeginTransmission(PCA9685_InitStruct->Address);
		PCA9685_I2C_Write(MODE1);
		PCA9685_I2C_Write((1 << MODE1_AI) | (1 << MODE1_ALLCALL));
		PCA9685_I2C_EndTransmission();

		/* MODE2 Register:
		 * Outputs change on STOP command
		 */
		uint8_t mode2 = (PCA9685_InitStruct->InvOutputs << MODE2_INVRT) |
		(PCA9685_InitStruct->OutputDriver << MODE2_OUTDRV) |
		(PCA9685_InitStruct->OutputNotEn << MODE2_OUTNE0);
		PCA9685_I2C_BeginTransmission(PCA9685_InitStruct->Address);
		PCA9685_I2C_Write(MODE2);
		PCA9685_I2C_Write(mode2);
		PCA9685_I2C_EndTransmission();

		/*
		// TESTING - 50% Duty. On at 0 and off at 2047 (0x7FF)
		PCA9685_I2C_BeginTransmission(PCA9685_InitStruct->Address);
		PCA9685_I2C_Write(ALL_LED_ON_L);
		PCA9685_I2C_Write(0x00);	// ALL_LED_ON_L
 		PCA9685_I2C_Write(0x00);	// ALL_LED_ON_H
 		PCA9685_I2C_Write(SERVO_TEST & 0xFF);	// ALL_LED_OFF_L
 		PCA9685_I2C_Write((SERVO_TEST >> 8) & 0xF);	// ALL_LED_OFF_H
		PCA9685_I2C_EndTransmission();

		PCA9685_I2C_BeginTransmission(PCA9685_InitStruct->Address);
		PCA9685_I2C_Write(ALL_LED_OFF_L);
		PCA9685_I2C_EndTransmission();
		uint8_t registerData = 0;
		PCA9685_I2C_RequestFrom(PCA9685_InitStruct->Address, &registerData, 1);
		*/

		//PCA9685_SetOutput(PCA9685_InitStruct->Address, 10, 0x0000, 205);
 	}
	
	return twiStatus;
}

/**
 * @brief	Sets a specific output for a PCA9685
 * @param	Address: The address to the PCA9685
 * @param	Output: The output to set
 * @param	OnValue: The value at which the output will turn on
 * @param	OffValue: The value at which the output will turn off
 * @retval	None
 */
void PCA9685_SetOutput(uint8_t Address, uint8_t Output, uint16_t OnValue, uint16_t OffValue)
{
	// Optional: PCA9685_I2C_SlaveAtAddress(Address), might make things slower
	if (Output <= MAX_OUTPUT_INDEX && OnValue <= MAX_OUTPUT_VALUE && OffValue <= MAX_OUTPUT_VALUE)
	{
		PCA9685_I2C_BeginTransmission(Address);
		PCA9685_I2C_Write(LEDn_ON_L(Output));
		PCA9685_I2C_Write(OnValue & 0xFF);			// LEDn_ON_L
		PCA9685_I2C_Write((OnValue >> 8) & 0xF);	// LEDn_ON_H
		PCA9685_I2C_Write(OffValue & 0xFF);			// LEDn_OFF_L
		PCA9685_I2C_Write((OffValue >> 8) & 0xF);	// LEDn_OFF_H
		PCA9685_I2C_EndTransmission();
	}
}

/**
 * @brief	Sets all outputs for a PCA9685
 * @param	Address: The address to the PCA9685
 * @param	OnValue: The value at which the outputs will turn on
 * @param	OffValue: The value at which the outputs will turn off
 * @retval	None
 */
void PCA9685_SetAll(uint8_t Address, uint16_t OnValue, uint16_t OffValue)
{
	if (OnValue <= MAX_OUTPUT_VALUE && OffValue <= MAX_OUTPUT_VALUE)
	{
		PCA9685_I2C_BeginTransmission(Address);
		PCA9685_I2C_Write(ALL_LED_ON_L);
		PCA9685_I2C_Write(OnValue & 0xFF);			// ALL_LED_ON_L
		PCA9685_I2C_Write((OnValue >> 8) & 0xF);	// ALL_LED_ON_H
		PCA9685_I2C_Write(OffValue & 0xFF);			// ALL_LED_OFF_L
		PCA9685_I2C_Write((OffValue >> 8) & 0xF);	// ALL_LED_OFF_H
		PCA9685_I2C_EndTransmission();
	}
}

/**
 * @brief	Sets a specific output for a PCA9685 based on an approximate duty cycle
 * @param	Address: The address to the PCA9685
 * @param	Output: The output to set
 * @param	DutyCycle: The duty cycle for the output
 * @retval	None
 */
void PCA9685_SetDutyCycleForOutput(uint8_t Address, uint8_t Output, uint8_t DutyCycle)
{
	uint16_t offValue = DutyCycle * 41; // 0 - 4100
	if (offValue > MAX_OUTPUT_VALUE) offValue = MAX_OUTPUT_VALUE;
	PCA9685_SetOutput(Address, Output, 0, offValue);
}


/**
 * @brief	Enter sleep mode
 * @param	Address: The address to the PCA9685
 * @retval	None
 */
void PCA9685_Sleep(uint8_t Address)
{
	/*
	PCA9685_I2C_BeginTransmission(Address);
	PCA9685_I2C_Write(MODE1);
	PCA9685_I2C_EndTransmission();
	uint8_t registerData = 0;
	PCA9685_I2C_RequestFrom(Address, &registerData, 1);

	registerData |= (1 << MODE1_SLEEP);
*/
	PCA9685_I2C_BeginTransmission(Address);
	PCA9685_I2C_Write(MODE1);
	PCA9685_I2C_Write(1 << MODE1_SLEEP);
	PCA9685_I2C_EndTransmission();
}

/**
 * @brief	Exit sleep mode
 * @param	Address: The address to the PCA9685
 * @retval	None
 */
void PCA9685_WakeUp(uint8_t Address)
{
	PCA9685_I2C_BeginTransmission(Address);
	PCA9685_I2C_Write(MODE1);
	PCA9685_I2C_EndTransmission();
	uint8_t registerData = 0;
	PCA9685_I2C_RequestFrom(Address, &registerData, 1);

	registerData &= ~(1 << MODE1_SLEEP);

	PCA9685_I2C_BeginTransmission(Address);
	PCA9685_I2C_Write(MODE1);
	PCA9685_I2C_Write(registerData);
	PCA9685_I2C_EndTransmission();
}

/* Interrupt Service Routines ------------------------------------------------*/
