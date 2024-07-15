/*
 * LM35.c
 *
 *  Created on: May 7, 2024
 *      Author: Rehab Khaled
 */

#include "LM35.h"
#include "adc.h"
//#include "stm32f1xx_hal_adc.h"


float LM35_fReadTemperature(void)
{
	uint16_t adc_value = 0;

	// Start ADC conversion
	if (HAL_ADC_Start(&hadc2) != HAL_OK) {
		// Handle ADC start error
		// Add error handling code here
		return 0.0f;
	}

	// Wait for ADC conversion to complete
	if (HAL_ADC_PollForConversion(&hadc2, 100) != HAL_OK) {
		// Handle ADC conversion error
		// Add error handling code here
		return 0.0f;
	}

	// Get ADC value
	adc_value = HAL_ADC_GetValue(&hadc2);

	// Assuming 3.3V Vref, 12-bit resolution
	float temperature = adc_value * 3.3f / 4096.0f;

	// LM35 output is 10mV/°C
	temperature *= 100.0f; // LM35 has a sensitivity of 10mV/°C

	HAL_ADC_Stop(&hadc2);

	return temperature;
}

