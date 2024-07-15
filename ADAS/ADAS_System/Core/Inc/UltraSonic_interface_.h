

#include "stm32f1xx_hal.h" // STM32 HAL library
#include "stm32f1xx_hal_tim.h" // STM32 HAL timer library

/**
 * @brief Delay in microseconds
 * @param time Delay time in microseconds
 */
void delay(uint16_t time);

/**
 * @brief Initialize the ultrasonic sensor
 */
void UltraSonic_INIT_TIMER1_ALL_CHANNEL(void);
void UltraSonic_INIT_TIMER3_Ch1_ch2(void);
/**
 * @brief Measure the distance using the ultrasonic sensor
 * @param Distance_CM Pointer to store the measured distance in centimeters
 */



void UltraSonic_Get_Distance_TIMER1_ALL_CHANNEL(float *Distance_CM_T1ch1 ,float *Distance_CM_T1ch2,float *Distance_CM_T1ch3,float *Distance_CM_T1ch4);
void UltraSonic_Get_Distance_TIMER3_Ch1_ch2(float *Distance_CM_T3ch1 ,float *Distance_CM_T3ch2);

