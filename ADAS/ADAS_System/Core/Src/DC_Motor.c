/*
 * DC_Motor.c
 *
 *  Created on: May 3, 2024
 *      Author: MohamedHusseinMohame
 */

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_gpio.h"
#include "DC_Motor.h"

extern TIM_HandleTypeDef htim2;


void DC_voidInit (void)
{
	//init PWM Timer1 for Back wheels
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}

void DC_voidMoveForward (uint8_t copy_u8Speed)
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, copy_u8Speed);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
}

void DC_voidMoveForwardRight (uint8_t copy_u8Speed)
{
	DC_voidTurnRight();
	DC_voidMoveForward(copy_u8Speed);
}

void DC_voidMoveForwardLeft (uint8_t copy_u8Speed)
{
	DC_voidTurnLeft();
	DC_voidMoveForward(copy_u8Speed);
}


void DC_voidMoveBackward (uint8_t copy_u8Speed)
{
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, copy_u8Speed);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
}

void DC_voidMoveBackwardRight (uint8_t copy_u8Speed)
{
	DC_voidTurnRight();
	DC_voidMoveBackward(copy_u8Speed);
}

void DC_voidMoveBackwardLeft (uint8_t copy_u8Speed)
{
	DC_voidTurnLeft();
	DC_voidMoveBackward(copy_u8Speed);
}


void DC_voidTurnRight (void)
{
	//__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 70);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}


void DC_voidTurnLeft (void)
{
	//__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 70);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}

void DC_voidNoTurn (void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}

void DC_voidStop(void)
{
	//HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

	//HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
}
