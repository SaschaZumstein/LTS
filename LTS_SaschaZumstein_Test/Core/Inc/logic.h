/**
 * @file	logic.h
 * @brief	Header file for signal processing functions of the LTS
 *
 * @author	Sascha Zumstein (FHGR)
 * @date	Last modified: 12.05.2025
 * @version 1.0
 */

#ifndef LOGIC_H
#define LOGIC_H

/*------------------------------------------------------------------------------------------*/
/* Includes                                                                                 */
/*------------------------------------------------------------------------------------------*/
#include <stdint.h>

/*------------------------------------------------------------------------------------------*/
/* Function prototypes                                                                      */
/*------------------------------------------------------------------------------------------*/
/**
 * @brief   Adjusts the shutter time based on minimum and maximum sensor values
 * @param   shutterTime: Pointer to the current shutter time
 * @param   minVal: Minimum pixel value measured
 * @param   maxVal: Maximum pixel value measured
 */
void logic_adjustShutterTime(uint16_t *shutterTime, uint16_t minVal, uint16_t maxVal);

/**
 * @brief   Calculates the distance based on the epc901 sensor data
 * @param   aquisitionData: Pointer to raw pixel values (1024 values)
 * @param   minVal: Minimum pixel value measured
 * @param   maxVal: Maximum pixel value measured
 * @return  Distance in millimeters on success, UINT16_MAX otherwise
 */
uint16_t logic_calcDist(uint16_t *aquisitionData, uint16_t minVal, uint16_t maxVal, uint16_t maxIndex);

/**
 * @brief	Sends data over the UART interfaces
 * @param   data: Pointer to the data buffer or string
 * @param   length Number of bytes to send
 */
void logic_writeData(const char *data, int length);

#endif /* LOGIC_H */
