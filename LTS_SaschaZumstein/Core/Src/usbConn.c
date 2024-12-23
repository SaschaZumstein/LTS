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
#include "usbConn.h"
#include <stdio.h>
#include <string.h>

/********************************************************************************************/
/* Defines                                                                       */
/********************************************************************************************/


/********************************************************************************************/
/* Type Definitions                                                                        */
/********************************************************************************************/

extern UART_HandleTypeDef huart2;

/********************************************************************************************/
/* Functions                                                                        */
/********************************************************************************************/
bool usb_hasConnection()
{
	// TODO check if connected or not
	return true;
}

void usb_writeMeasurements(uint16_t distance)
{
	char sendData[11];
	sprintf(sendData, "%d mm\n\r", distance);
	UART2_Send(sendData);
}

void UART2_Send(const char *data){
	HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), HAL_MAX_DELAY);
}

bool UART2_Receive(char *buffer, size_t buffer_size){
	memset(buffer, 0, buffer_size);
	return HAL_UART_Receive(&huart2, (uint8_t *)buffer, buffer_size, 1000) == HAL_OK;
}

