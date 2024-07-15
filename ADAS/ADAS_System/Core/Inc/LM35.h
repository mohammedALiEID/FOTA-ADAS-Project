/*
 * LM35.h
 *
 *  Created on: May 7, 2024
 *      Author: Rehab Khaled
 */

#ifndef INC_LM35_H_
#define INC_LM35_H_

#include "stm32f1xx_hal.h"

extern ADC_HandleTypeDef hadc2;

float LM35_fReadTemperature(void);


#endif /* INC_LM35_H_ */
