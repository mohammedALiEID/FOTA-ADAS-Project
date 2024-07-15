/*
 * BLU.C
 *
 *  Created on: Apr 2, 2024
 *      Author: Roqaia Khalid
 */
#include "BLU.h"
#include "main.h"

/**
  * @brief  Receives data from the Bluetooth module in polling mode.
  * @param  pData Pointer to the buffer where received data will be stored.
  * @param  Size  Number of bytes to receive.
  * @param  Timeout Timeout duration in milliseconds.
  * @retval HAL status.
  */
HAL_StatusTypeDef BLU_voidReceivePolling(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    HAL_StatusTypeDef Local_u8Status  = HAL_OK;

    // Call HAL_UART_Receive function to receive data from the Bluetooth module
    Local_u8Status = HAL_UART_Receive(&huart2, pData, Size, Timeout);

    if(Local_u8Status != HAL_OK)
        {
    	Local_u8Status = HAL_ERROR;
        }
    else
        {

        }

        return Local_u8Status;
}


