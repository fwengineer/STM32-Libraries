/**
 ******************************************************************************
 * @file	hex_servo.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-09
 * @brief	Manage servos
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HEX_SERVO_H_
#define HEX_SERVO_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Defines -------------------------------------------------------------------*/
#define SERVO_500_50HZ		103
#define SERVO_1000_50HZ		205
#define SERVO_1500_50HZ		308
#define SERVO_2000_50HZ		410

#define SERVO_MIN			750
#define SERVO_CENTER		1500
#define SERVO_MAX			2250

#define SERVO_X_50HZ(X)		(uint16_t)(0.2048*X + 0.5)

#define NUM_OF_SERVOS		22

/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void HEX_SERVO_Init();
void HEX_SERVO_Update();

void HEX_SERVO_MoveToPosition(uint8_t Servo, uint32_t Position, uint32_t TimeToReachPosition);


void HEX_SERVO_SetRaw(uint8_t Servo, uint16_t Value);
void HEX_SERVO_SetAllRaw(uint16_t Value);

void HEX_SERVO_EnablePwr();
void HEX_SERVO_DisablePwr();
void HEX_SERVO_TogglePwr();

#endif /* HEX_SERVO_H_ */
