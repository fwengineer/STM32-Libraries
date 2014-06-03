/**
 ******************************************************************************
 * @file	color.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2014-06-02
 * @brief
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "color.h"

/* Private defines -----------------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Convert HSB to RGB565
 * @param	HSB: Pointer to a HSB struct
 * @param	RGB: Pointer to a RGB565 struct
 * @retval	None
 */
void COLOR_HSBtoRGB565(HSB_TypeDef* HSB, RGB565_TypeDef* RGB)
{
	float saturation = HSB->saturation;
	float brightness = HSB->brightness;
	float h_i, f, p, q, t, R, G, B;

	uint16_t hue = HSB->hue % 360;
	h_i = hue / 60;
	f = (float)(hue) / 60.0 - h_i;
	p = brightness * (1 - saturation);
	q = brightness * (1 - saturation * f);
	t = brightness * (1 - saturation * (1 - f));

	if (h_i == 0)
	{
		R = brightness;
		G = t;
		B = p;
	}
	else if (h_i == 1)
	{
		R = q;
		G = brightness;
		B = p;
	}
	else if (h_i == 2)
	{
		R = p;
		G = brightness;
		B = t;
	}
	else if (h_i == 3)
	{
		R = p;
		G = q;
		B = brightness;
	}
	else if (h_i == 4)
	{
		R = t;
		G = p;
		B = brightness;
	}
	else {
		R = brightness;
		G = p;
		B = q;
	}

	RGB->red = (uint8_t)(R * 31.0);
	RGB->green = (uint8_t)(G * 63.0);
	RGB->blue = (uint8_t)(B * 31.0);
}

/* Private functions ---------------------------------------------------------*/
/* Interrupt Handlers --------------------------------------------------------*/
