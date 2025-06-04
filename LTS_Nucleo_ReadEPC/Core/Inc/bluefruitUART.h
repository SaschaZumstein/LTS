/*
 * bluefruitUART.h
 *
 *  Created on: 13.11.2024
 *  Author: Sascha Zumstein (FHGR)
 */

#ifndef BLUEFRUIT_UART_H
#define BLUEFRUIT_UART_H

#include <stdbool.h>

/* Private function prototypes -----------------------------------------------*/
bool bluefruit_hasConnection();
void bluefruit_writeMeasurements(uint16_t distance);
void UART_Send(const char *data);
bool UART_Receive(char *buffer, size_t buffer_size);

#endif /* CCD_EPC_H_ */
