/*
 * bluefruitUART.h
 *
 *  Created on: 13.11.2024
 *  Author: Sascha Zumstein (FHGR)
 */

#ifndef CONN_H
#define CONN_H

#include <stdbool.h>

/* Private function prototypes -----------------------------------------------*/
bool bluefruit_hasConnection();
bool usb_hasConnection();
void conn_writeData(const char *data, int length, bool bluetooth, bool usb);


#endif /* CCD_EPC_H_ */
