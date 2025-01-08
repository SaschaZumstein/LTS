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
	// TODO check if connected or not
	return true;
}

// Write data via bluetooth
void bluefruit_writeMeasurements(uint16_t distance)
{
	char sendData[10];
	sprintf(sendData, "%d mm\n", distance);
	HAL_UART_Transmit(&huart1, (uint8_t *)sendData , strlen(sendData), HAL_MAX_DELAY);
}

void bluefruit_writeOff(void)
{
	const char sendData[11] = "Laser aus\n";
	HAL_UART_Transmit(&huart1, (uint8_t *)sendData , strlen(sendData), HAL_MAX_DELAY);
}

