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
void usb_writeOff(void);

#endif /* INC_USBCONN_H_ */
