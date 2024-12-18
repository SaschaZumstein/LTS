/*
This file contains the initialization and the read and write functions for the bluefruit UART module

  * Author: Sascha Zumstein (FHGR)
  * Date: 13.11.2024
  * Version: 1.0
  * Changes:
  * 13.11.2024 V1.0 Initial Version
*/


/********************************************************************************************/
/* Includes                                                                                 */
/********************************************************************************************/

#include "main.h"
#include "bluefruitUART.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/********************************************************************************************/
/* Defines                                                                       */
/********************************************************************************************/


/********************************************************************************************/
/* Type Definitions                                                                        */
/********************************************************************************************/

extern UART_HandleTypeDef huart1;

/********************************************************************************************/
/* Functions                                                                        */
/********************************************************************************************/
bool bluefruit_hasConnection()
{
	return true;
}

// Write data via bluetooth
void bluefruit_writeMeasurements(uint16_t distance)
{
	char sendData[10];
	sprintf(sendData, "%d mm\n", distance);
	UART1_Send(sendData);
}

void UART1_Send(const char *data) {
    HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), HAL_MAX_DELAY);
}

bool UART1_Receive(char *buffer, size_t buffer_size) {
    memset(buffer, 0, buffer_size);
    return HAL_UART_Receive(&huart1, (uint8_t *)buffer, buffer_size, 1000) == HAL_OK;
}

