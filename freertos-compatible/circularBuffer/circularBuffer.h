/**
 ******************************************************************************
 * @file	circularBuffer.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-03-01
 * @brief	A circular buffer with functionality to insert and remove items
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "common.h"

/* Defines -------------------------------------------------------------------*/
#ifndef CIRCULARBUFFER_SIZE
#warning "CIRCULARBUFFER_SIZE is not defined. Will now define it to 64"
#define CIRCULARBUFFER_SIZE	64
#endif

#ifndef CIRCULARBUFFER_DATATYPE
#warning "CIRCULARBUFFER_DATATYPE is not defined. Will now define it to uint32_t"
#define CIRCULARBUFFER_DATATYPE	uint8_t
#endif

#if (CIRCULARBUFFER_SIZE <= 0xFF)
#define CIRCULARBUFFER_COUNTTYPE    uint8_t
#else
#define CIRCULARBUFFER_COUNTTYPE    uint16_t
#endif
/* Typedefs ------------------------------------------------------------------*/
/**
 * @brief  Struct to handle a circular buffer
 */
typedef struct
{
	CIRCULARBUFFER_DATATYPE *in;							/** Pointer that points at the head where 
																data should be written */
	CIRCULARBUFFER_DATATYPE *out;							/** Pointer that points at the tail where 
																data should be read */
	CIRCULARBUFFER_DATATYPE data[CIRCULARBUFFER_SIZE];		/** The actual buffer */
	CIRCULARBUFFER_COUNTTYPE count;							/** A counter for how much data there is
																in the buffer */
	Boolean Initialized;									/** Variable that is set once init has been
																done on the buffer */
} CircularBuffer_TypeDef;

/* Function prototypes -------------------------------------------------------*/
void CIRC_BUFFER_Init(CircularBuffer_TypeDef* CircularBuffer);
void CIRC_BUFFER_Insert(CircularBuffer_TypeDef* CircularBuffer, CIRCULARBUFFER_DATATYPE Data);
CIRCULARBUFFER_DATATYPE CIRC_BUFFER_Remove(CircularBuffer_TypeDef* CircularBuffer);
CIRCULARBUFFER_COUNTTYPE CIRC_BUFFER_GetCount(CircularBuffer_TypeDef* CircularBuffer);
uint8_t CIRC_BUFFER_IsEmpty(CircularBuffer_TypeDef* CircularBuffer);
uint8_t CIRC_BUFFER_IsFull(CircularBuffer_TypeDef* CircularBuffer);

#endif /* CIRCULARBUFFER_H_ */
