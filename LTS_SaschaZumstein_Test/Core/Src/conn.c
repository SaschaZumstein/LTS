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
#include "conn.h"
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
extern UART_HandleTypeDef huart2;

/********************************************************************************************/
/* Functions                                                                        */
/********************************************************************************************/
bool bluefruit_hasConnection()
{
	// TODO check if connected or not
	return true;
}

bool usb_hasConnection()
{
	// TODO check if connected or not
	return true;
}

// Write data
void conn_writeData(const char *data, int length, bool bluetooth, bool usb)
{
	if(bluetooth){
		HAL_UART_Transmit(&huart1, (uint8_t *)data, length, HAL_MAX_DELAY);
	}
	if(usb){
		HAL_UART_Transmit(&huart2, (uint8_t *)data, length, HAL_MAX_DELAY);
	}
}
