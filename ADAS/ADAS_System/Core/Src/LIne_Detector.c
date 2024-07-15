/*
 * LIne_Detector.c
 *
 *  Created on: May 1, 2024
 *      Author: eqw18
 */

#include "../Inc/Line_Detector.h"
#include "../../Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal.h"

#define Line_Tracker_Port	GPIOB
#define Line_Tracker_Pin1 	GPIO_PIN_10
#define Line_Tracker_Pin2 	GPIO_PIN_11

void LineDetector_voidRead(uint8_t *IR1, uint8_t *IR2)
{
	*IR1 = HAL_GPIO_ReadPin(Line_Tracker_Port, Line_Tracker_Pin1);
	*IR2 = HAL_GPIO_ReadPin(Line_Tracker_Port, Line_Tracker_Pin2);
}
