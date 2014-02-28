/**
 ******************************************************************************
 * @file	hex_servo.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-09
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "pca9685.h"
#include "millis.h"
#include "hex_servo.h"

/* Private Defines -----------------------------------------------------------*/
#define POWER_PIN	GPIO_Pin_9
#define POWER_GPIO	GPIOB

#define OUTPUT_ENABLE_PIN	GPIO_Pin_1
#define OUTPUT_ENABLE_GPIO	GPIOB

#define PCA9685_IC1		0x43
#define PCA9685_IC2		0x41

/* Private Typedefs ----------------------------------------------------------*/
typedef struct
{
	uint32_t currentPosition;
	uint32_t goalPosition;
	uint32_t timeLeftToReachGoal;
	uint32_t offsetPosition;
} Servo_TypeDef;

/* Private Variables ---------------------------------------------------------*/
const uint8_t _servoOutput[22] = {10, 11, 12, 13, 14, 15, 0,  1, 2, 5, 4,
								   3,  2,  1,  0, 15, 14, 13, 9, 8, 7, 6};
const uint8_t _servoIc[22] = {PCA9685_IC1, PCA9685_IC1, PCA9685_IC1, PCA9685_IC1,
								PCA9685_IC1, PCA9685_IC1, PCA9685_IC2, PCA9685_IC2,
								PCA9685_IC2, PCA9685_IC1, PCA9685_IC1, PCA9685_IC1,
								PCA9685_IC1, PCA9685_IC1, PCA9685_IC1, PCA9685_IC2,
								PCA9685_IC2, PCA9685_IC2, PCA9685_IC1, PCA9685_IC1,
								PCA9685_IC1, PCA9685_IC1};

Servo_TypeDef _servos[NUM_OF_SERVOS];

/* Private Functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the servo control
 * @param	None
 * @retval	None
 */
void HEX_SERVO_Init()
{
	Servo_TypeDef resetServo = {SERVO_CENTER, SERVO_CENTER, 0, 0};

	uint32_t i;
	for (i = 0; i < NUM_OF_SERVOS; i++)
	{
		// Read offset from EEPROM
		//uint32_t offset = EEPROM_GetOffset(i);
		//resetServo.offsetPosition = offset;

		_servos[i] = resetServo;
	}

	// Enable clock for GPIOB
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = POWER_PIN | OUTPUT_ENABLE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	PCA9685_Init_TypeDef pca9685Init;
	// IC1
	pca9685Init.Address = 0x43;
	pca9685Init.InvOutputs = PCA9685_NotInvOutputs;
	pca9685Init.OutputDriver = PCA9685_OutputDriver_TotemPole;
	pca9685Init.OutputNotEn = PCA9685_OutputNotEn_High_Z2;
	pca9685Init.PWMFrequency = PCA9685_Frequency_50Hz;
	PCA9685_Init(&pca9685Init);

	// IC2
	pca9685Init.Address = 0x41;
	PCA9685_Init(&pca9685Init);

	HEX_SERVO_EnablePwr();
}

/**
 * @brief	Update the position of the servos
 * @param	None
 * @retval	None
 */
void HEX_SERVO_Update()
{
	static uint32_t lastUpdate = 0;
	uint32_t timeSinceLastUpdate = millis() - lastUpdate;

	uint32_t servo;
	for (servo = 0; servo < NUM_OF_SERVOS; servo++)
	{
		/* Calculate new position */
		if (_servos[servo].timeLeftToReachGoal == 0)
		{
			_servos[servo].currentPosition = _servos[servo].goalPosition;
		}
		else
		{
			int32_t difference = _servos[servo].goalPosition - _servos[servo].currentPosition;
			int32_t stepDivider = _servos[servo].timeLeftToReachGoal / timeSinceLastUpdate;
			int32_t step = difference / stepDivider;
			_servos[servo].currentPosition += step;

			if (_servos[servo].timeLeftToReachGoal - timeSinceLastUpdate < 0) _servos[servo].timeLeftToReachGoal = 0;
			else _servos[servo].timeLeftToReachGoal -= timeSinceLastUpdate;
		}

		/* Set outputs */
		PCA9685_SetOutput(_servoIc[servo], _servoOutput[servo], 0x0000, SERVO_X_50HZ(_servos[servo].currentPosition + _servos[servo].offsetPosition));
	}
	lastUpdate = millis();
}

/**
 * @brief	Move a servo to a position within a certain timeperiod
 * @param	Servo: The servo to move (1 to 22)
 * @param	Position: The position to move to
 * @param	TimeToReachPosition: The time the movement should take in ms
 * @retval	None
 */
void HEX_SERVO_MoveToPosition(uint8_t Servo, uint32_t Position, uint32_t TimeToReachPosition)
{
	if (Servo > 0 && Servo <= NUM_OF_SERVOS && Position >= SERVO_MIN && Position <= SERVO_MAX)
	{
		_servos[Servo-1].goalPosition = Position;
		_servos[Servo-1].timeLeftToReachGoal = TimeToReachPosition;
	}
}

/**
 * @brief	Set a servo to a specific value immediately
 * @param	Servo: The servo to set
 * @param	Value: The value
 * @retval	None
 */
void HEX_SERVO_SetRaw(uint8_t Servo, uint16_t Value)
{
	if (Servo > 0 && Servo <= NUM_OF_SERVOS && Value >= SERVO_MIN && Value <= SERVO_MAX)
	{
		PCA9685_SetOutput(_servoIc[Servo-1], _servoOutput[Servo-1], 0x0000, SERVO_X_50HZ(Value));
	}
}

/**
 * @brief	Set all servos to a specific value immediately
 * @param	Value: The value
 * @retval	None
 */
void HEX_SERVO_SetAllRaw(uint16_t Value)
{
	PCA9685_SetAll(PCA9685_IC1, 0x0000, SERVO_X_50HZ(Value));
	PCA9685_SetAll(PCA9685_IC2, 0x0000, SERVO_X_50HZ(Value));
}

/**
 * @brief	Enables the servo power
 * @param	None
 * @retval	None
 */
void HEX_SERVO_EnablePwr()
{
	GPIO_SetBits(POWER_GPIO, POWER_PIN);
	//uint32_t delay = millis();
	//while (millis() - delay <= 100);
	GPIO_ResetBits(OUTPUT_ENABLE_GPIO, OUTPUT_ENABLE_PIN);
}

/**
 * @brief	Disables the servo power
 * @param	None
 * @retval	None
 */
void HEX_SERVO_DisablePwr()
{
	GPIO_ResetBits(POWER_GPIO, POWER_PIN);
	GPIO_SetBits(OUTPUT_ENABLE_GPIO, OUTPUT_ENABLE_PIN);
}

/**
 * @brief	Toggles the servo power
 * @param	None
 * @retval	None
 */
void HEX_SERVO_TogglePwr()
{
	POWER_GPIO->ODR ^= (POWER_PIN | OUTPUT_ENABLE_PIN);
}

/* Interrupt Handlers --------------------------------------------------------*/
