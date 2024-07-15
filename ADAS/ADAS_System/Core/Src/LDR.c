/*
 * LDR.c
 *
 *  Created on: May 7, 2024
 *      Author: eqw18
 */

#include "../Inc/LDR.h"
extern ADC_HandleTypeDef hadc1;

uint16_t HAL_u8LDRRead(void){/*	if read is below 4000 switch ON the lights 	*/
	uint16_t read_value = 0;

	HAL_ADC_Start(&hadc1);
	if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) { // Adjust poll time as needed
		read_value = HAL_ADC_GetValue(&hadc1);
	}
	HAL_ADC_Stop(&hadc1);

	return read_value;
}
