/**
 ******************************************************************************
 * @file	messages.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-10-26
 * @brief
 ******************************************************************************
	Copyright (c) 2014 Hampus Sandberg.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation, either
	version 3 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MESSAGES_H_
#define MESSAGES_H_

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
typedef enum
{
	HomeAutomationMessage_LedStrip = 0xAABBCCDD,
} HomeAutomationMessage;


/* LED Strip ================================================================ */
typedef enum
{
	LedStripChannel_1 = 0x01,
	LedStripChannel_2 = 0x02,
	LedStripChannel_All = 0xFF,
} LedStripChannel;

typedef enum
{
	LedStripMode_Off,
	LedStripMode_On,
} LedStripMode;

typedef struct
{
	uint16_t red;
	uint16_t green;
	uint16_t blue;
	uint16_t white;
} LedStripColorValues;

typedef enum
{
	LedStripColorLimit_Min = 0,
	LedStripColorLimit_Max = 4095,
} LedStripColorLimit;

typedef struct
{
	HomeAutomationMessage messageType;		/* The message type should always be first in message structs */

	LedStripChannel channel;
	LedStripMode mode;
	LedStripColorValues colors;
} LedStripMessage;

/* Function prototypes -------------------------------------------------------*/


#endif /* MESSAGES_H_ */
