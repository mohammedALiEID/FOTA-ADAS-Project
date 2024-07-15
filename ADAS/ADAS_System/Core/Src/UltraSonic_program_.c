/*
 * UltraSonic_program_.c
 *
 *  Created on: 5/5/2024
 *      Author:mohammed ali
 */

#include <UltraSonic_interface_.h>
#include "tim.h"
#include "gpio.h"
/***********************************TIMER1*************************************************/
static void ICU_VoidGet_TON_TIMER1_ALL_CHANNEL(uint32_t *TON_microSec1 ,uint32_t *TON_microSec2,uint32_t *TON_microSec3,uint32_t *TON_microSec4) ;

volatile uint16_t TIMEON1 = 0; // Variable to store the time-on period of the ultrasonic pulse
volatile uint16_t TIMEON2 = 0; // Variable to store the time-on period of the ultrasonic pulse
volatile uint16_t TIMEON3 = 0; // Variable to store the time-on period of the ultrasonic pulse
volatile uint16_t TIMEON4 = 0; // Variable to store the time-on period of the ultrasonic pulse
volatile uint8_t IT_numbers = 0 ;
volatile uint8_t State = 0; // Variable to keep track of the state of the ultrasonic measurement

/***********************************TIMER3*************************************************/
static void ICU_VoidGet_TON_TIMER3_Ch1_ch2(uint32_t *TON_microSec1 ,uint32_t *TON_microSec2) ;
volatile uint16_t TIMEON1_T3 = 0; // Variable to store the time-on period of the ultrasonic pulse
volatile uint16_t TIMEON2_T3 = 0; // Variable to store the time-on period of the ultrasonic pulse
volatile uint8_t IT_numbers_T3 = 0 ;
volatile uint8_t State_T3 = 0;
const uint16_t velocity = 343; // Speed of sound in air in meters per second

/**
 * @brief Delay function in microseconds
 * @param time Delay time in microseconds
 */
void delay(uint16_t time) {
	__HAL_TIM_SET_COUNTER(&htim1, 0); // Reset the timer counter
	while (__HAL_TIM_GET_COUNTER(&htim1) < time); // Wait until the timer counter reaches the specified time
}

/**
 * @brief This function handles the TIM1 capture compare interrupt.
 * It's called when an input capture event occurs on TIM1 channel 1.
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	/* USER CODE BEGIN voidTIM1_CC_IRQn 0 */



	if (htim->Instance == TIM1) {
		// This interrupt is from TIM1



		if (State == 0) {
			// Reset the timer counter and capture/compare register 1
			TIM1->CNT = 0x0;

			if (__HAL_TIM_GET_FLAG(&htim1 ,TIM_FLAG_CC1)){
				TIM1->CCR1 = 0x0;
				// Set the input capture edge to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
				__HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC1);
				IT_numbers++ ;


			} else{}



			if (__HAL_TIM_GET_FLAG(&htim1 ,TIM_FLAG_CC2)){
				TIM1->CCR2 = 0x0;
				// Set the input capture edge to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
				__HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC2);
				IT_numbers++ ;


			} else{}
			if (__HAL_TIM_GET_FLAG(&htim1 ,TIM_FLAG_CC3)){
				TIM1->CCR3 = 0x0;
				// Set the input capture edge to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
				__HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC3);
				IT_numbers++ ;


			} else{}
			if (__HAL_TIM_GET_FLAG(&htim1 ,TIM_FLAG_CC4)){
				TIM1->CCR4 = 0x0;
				// Set the input capture edge to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_FALLING);
				__HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC4);
				IT_numbers++ ;


			} else{}


			if(IT_numbers==4){

				State++ ;
				IT_numbers = 0 ;


			}else{} ;


		} else if (State == 1) {
			// Store the time-on period when the rising edge is detected




			if (__HAL_TIM_GET_FLAG(&htim1 ,TIM_FLAG_CC1)){
				TIMEON1 = TIM1->CCR1;
				// Set the input capture edge to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
				IT_numbers++ ;
				__HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC1);

			} else{}

			if (__HAL_TIM_GET_FLAG(&htim1 ,TIM_FLAG_CC2)){
				TIMEON2 = TIM1->CCR2;
				// Set the input capture edge to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
				IT_numbers++ ;
				__HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC2);

			} else{}
			if (__HAL_TIM_GET_FLAG(&htim1 ,TIM_FLAG_CC3)){
				TIMEON3 = TIM1->CCR3;
				// Set the input capture edge to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
				IT_numbers++ ;
				__HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC3);

			} else{}
			if (__HAL_TIM_GET_FLAG(&htim1 ,TIM_FLAG_CC4)){
				TIMEON4 = TIM1->CCR4;
				// Set the input capture edge to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_RISING);
				IT_numbers++ ;
				__HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_CC4);

			} else{}

			if(IT_numbers==4){

				State++ ;


			}
		}



		else {
			// Do nothing
		}






		/********************************************************TIMER3******************************************/




	} else if (htim->Instance == TIM3) {
		// This interrupt is from TIM3




		if (State_T3 == 0) {
			// Reset the timer counter and capture/compare register 1
			TIM3->CNT = 0x0;

			if (__HAL_TIM_GET_FLAG(&htim3 ,TIM_FLAG_CC1)){
				TIM3->CCR1 = 0x0;
				// Set the input capture edge to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
				__HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_CC1);
				IT_numbers_T3++ ;

			} else{}


			if (__HAL_TIM_GET_FLAG(&htim3 ,TIM_FLAG_CC2)){
				TIM3->CCR2 = 0x0;
				// Set the input capture edge to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
				__HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_CC2);
				IT_numbers_T3++ ;


			} else{}
			if(IT_numbers_T3==2){

				State_T3++ ;
				IT_numbers_T3 = 0 ;


			}else{}



		}

		else if (State_T3==1){

			if (__HAL_TIM_GET_FLAG(&htim3 ,TIM_FLAG_CC1)){
				TIMEON1_T3 = TIM3->CCR1;
				// Set the input capture edge to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
				IT_numbers_T3++ ;
				__HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_CC1);

			} else{}

			if (__HAL_TIM_GET_FLAG(&htim3 ,TIM_FLAG_CC2)){
				TIMEON2_T3 = TIM3->CCR2;
				// Set the input capture edge to rising edge
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
				IT_numbers_T3++ ;
				__HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_CC2);

			} else{}


			if(IT_numbers_T3==2){

				State_T3++ ;


			}


		}

		else {/*Do nothing*/ }



	}



}


/**
 * @brief Internal function to get the time-on period
 * @param Copy_u32_TON_microSec Pointer to store the time-on period in microseconds
 */
void ICU_VoidGet_TON_TIMER1_ALL_CHANNEL(uint32_t *TON_microSec1 ,uint32_t *TON_microSec2,uint32_t *TON_microSec3,uint32_t *TON_microSec4) {

	if (State == 2) // Wait until the measurement is complete
	{
		// Store the time-on period
		*TON_microSec1 = TIMEON1;
		*TON_microSec2 = TIMEON2;
		*TON_microSec3 = TIMEON3;
		*TON_microSec4 = TIMEON4;

		// Reset the state for the next measurement
		State = 0 ;
		IT_numbers = 0 ;
	}
}
void ICU_VoidGet_TON_TIMER3_Ch1_ch2(uint32_t *TON_microSec1 ,uint32_t *TON_microSec2) {

	if (State_T3 == 2)
	{

		// Store the time-on period
		*TON_microSec1 = TIMEON1_T3;
		*TON_microSec2 = TIMEON2_T3;


		// Reset the state for the next measurement
		State_T3 = 0 ;
		IT_numbers_T3 = 0 ;

		// Wait until the measurement is complete
	}


}
/**
 * @brief Initialize the ultrasonic sensor
 */
void UltraSonic_INIT_TIMER1_ALL_CHANNEL(void) {
	// Start the input capture interrupt for TIM1 all channel
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2) ;
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_4) ;

}
void UltraSonic_INIT_TIMER3_Ch1_ch2(void){

	// Start the input capture interrupt for TIM3 channel 1 ,2
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2) ;

}
/**
 * @brief Measure the distance using the ultrasonic sensor
 * @param Distance_CM Pointer to store the measured distance in centimeters
 */
void UltraSonic_Get_Distance_TIMER1_ALL_CHANNEL(float *Distance_CM_T1ch1 ,float *Distance_CM_T1ch2,float *Distance_CM_T1ch3,float *Distance_CM_T1ch4) {
	uint32_t TON_microSec1 = 0;
	uint32_t TON_microSec2 = 0;
	uint32_t TON_microSec3 = 0;
	uint32_t TON_microSec4 = 0;
	// Trigger the ultrasonic sensor by sending a pulse
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	delay(3); // Wait for a short duration
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	delay(10); // Keep the trigger pin high for the required duration
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); // Reset the trigger pin

	// Measure the time-on period using the internal function
	ICU_VoidGet_TON_TIMER1_ALL_CHANNEL(&TON_microSec1,&TON_microSec2,&TON_microSec3,&TON_microSec4);

	// Calculate the distance in centimeters using the time-on period and the speed of sound
	*Distance_CM_T1ch1 = (float)(((velocity * TON_microSec1) / 10000.0) / 2.0); // CENTIMETER
	*Distance_CM_T1ch2 = (float)(((velocity * TON_microSec2) / 10000.0) / 2.0); // CENTIMETER
	*Distance_CM_T1ch3 = (float)(((velocity * TON_microSec3) / 10000.0) / 2.0); // CENTIMETER
	*Distance_CM_T1ch4 = (float)(((velocity * TON_microSec4) / 10000.0) / 2.0); // CENTIMETER


}

void UltraSonic_Get_Distance_TIMER3_Ch1_ch2(float *Distance_CM_T3ch1 ,float *Distance_CM_T3ch2){

	uint32_t TON_microSec1 = 0;
	uint32_t TON_microSec2 = 0;
	// Trigger the ultrasonic sensor by sending a pulse
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	delay(3); // Wait for a short duration
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
	delay(10); // Keep the trigger pin high for the required duration
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET); // Reset the trigger pin

	ICU_VoidGet_TON_TIMER3_Ch1_ch2(&TON_microSec1,&TON_microSec2) ;

	// Calculate the distance in centimeters using the time-on period and the speed of sound
	*Distance_CM_T3ch1 = (float)(((velocity * TON_microSec1) / 10000.0) / 2.0); // CENTIMETER
	*Distance_CM_T3ch2 = (float)(((velocity * TON_microSec2) / 10000.0) / 2.0); // CENTIMETER
}
