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
	sprintf(sendData, "%d mm\r\n", distance);
	HAL_UART_Transmit(&huart2, (uint8_t *)sendData , strlen(sendData), HAL_MAX_DELAY);
}

void usb_writeOff(void)
{
	const char sendData[12] = "Laser aus\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t *)sendData , strlen(sendData), HAL_MAX_DELAY);
}


