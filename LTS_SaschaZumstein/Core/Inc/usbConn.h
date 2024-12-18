/*
 * usbConn.h
 *
 *  Created on: Dec 18, 2024
 *      Author: zumst
 */

#ifndef INC_USBCONN_H_
#define INC_USBCONN_H_

#include <stdbool.h>

/* Private function prototypes -----------------------------------------------*/
bool usb_hasConnection();
void usb_writeMeasurements(uint16_t distance);
void UART2_Send(const char *data);
bool UART2_Receive(char *buffer, size_t buffer_size);

#endif /* INC_USBCONN_H_ */
