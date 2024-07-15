/*
 * DC_Motor.h
 *
 *  Created on: May 3, 2024
 *      Author: MohamedHusseinMohame
 */

#ifndef INC_DC_MOTOR_H_
#define INC_DC_MOTOR_H_

void DC_voidInit (void);


void DC_voidMoveForward (uint8_t copy_u8Speed);
void DC_voidMoveForwardRight (uint8_t copy_u8Speed);
void DC_voidMoveForwardLeft (uint8_t copy_u8Speed);

void DC_voidMoveBackward (uint8_t copy_u8Speed);
void DC_voidMoveBackwardRight (uint8_t copy_u8Speed);
void DC_voidMoveBackwardLeft (uint8_t copy_u8Speed);

void DC_voidTurnLeft (void);
void DC_voidTurnRight (void);
void DC_voidNoTurn (void);


void DC_voidStop(void);


#endif /* INC_DC_MOTOR_H_ */
