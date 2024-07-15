/*
 * BLU.h
 *
 *  Created on: Apr 2, 2024
 *      Author: Roqaia Khalid
 */

#ifndef INC_BLU_H_
#define INC_BLU_H_
#include "main.h"

extern UART_HandleTypeDef huart2;

// Function prototypes

HAL_StatusTypeDef BLU_voidReceivePolling(uint8_t *pData, uint16_t Size, uint32_t Timeout);

#endif /* INC_BLU_H_ */
