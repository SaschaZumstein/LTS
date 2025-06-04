/**
 * @file	logic.c
 * @brief	Implementation of the signal processing functions of the LTS
 *
 * @author	Sascha Zumstein (FHGR)
 * @date	Last modified: 12.05.2025
 * @version 1.0
 */

/*------------------------------------------------------------------------------------------*/
/* Includes                                                                                 */
/*------------------------------------------------------------------------------------------*/
#include "main.h"
#include "logic.h"
#include <stdio.h>
#include <stdbool.h>

/*------------------------------------------------------------------------------------------*/
/* Defines                                                                                  */
/*------------------------------------------------------------------------------------------*/
#define NUM_OF_PIX 				1024
#define MIN_BASELINE_PEAK 		(40<<4)
#define MAX_BASELINE_PEAK 		(70<<4)
#define MIN_BASELINE_NO_PEAK	(70<<4)
#define MAX_BASELINE_NO_PEAK 	(100<<4)
#define MIN_PEAK 				(80<<4)
#define MAX_PEAK 				(115<<4)
#define MIN_PEAK_HEIGHT 		(15<<4)
#define MIN_SHUTTER 			0
#define MAX_SHUTTER 			12
#define MIN_COG					31.1
#define MAX_COG		 			975
#define INDEX_THRESHOLD			50
#define BUFFER_SIZE				5

/*------------------------------------------------------------------------------------------*/
/* External Handles                                                                         */
/*------------------------------------------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/*------------------------------------------------------------------------------------------*/
/* Functions                                                                                */
/*------------------------------------------------------------------------------------------*/
/**
 * @brief   Adjusts the shutter time based on minimum and maximum sensor values
 * @param   shutterTime: Pointer to the current shutter time
 * @param   minVal: Minimum pixel value measured
 * @param   maxVal: Maximum pixel value measured
 */
void logic_adjustShutterTime(uint16_t *shutterTime, uint16_t minVal, uint16_t maxVal)
{
	const uint16_t shutterList[] = {50, 70, 90, 125, 165, 225, 300, 410, 550, 750, 1000, 1500, 2250};
	static uint8_t shutterIndex = 3;

	// no peak detected => bring baseline to a good value
	if(maxVal < (minVal + MIN_PEAK_HEIGHT)) {
		if(minVal < MIN_BASELINE_NO_PEAK && shutterIndex < MAX_SHUTTER){
			shutterIndex++;
		}
		else if(minVal > MAX_BASELINE_NO_PEAK && shutterIndex > MIN_SHUTTER){
			shutterIndex--;
		}
	}
	// peak detected => search optimum peak to baseline ratio
	else {
		// baseline or peak to high => reduce shutter time
		if((minVal > MAX_BASELINE_PEAK || maxVal > MAX_PEAK) && shutterIndex > MIN_SHUTTER){
			shutterIndex--;
		}
		// peak to low and baseline ok => increase shutter time
		else if (minVal < MIN_BASELINE_PEAK && maxVal < MIN_PEAK && shutterIndex < MAX_SHUTTER) {
			shutterIndex++;
		}
	}
	// calculate new shutter time
	(*shutterTime) = shutterList[shutterIndex];
}

/**
 * @brief   Calculates the distance based on the epc901 sensor data
 * @param   aquisitionData: Pointer to raw pixel values (1024 values)
 * @param   minVal: Minimum pixel value measured
 * @param   maxVal: Maximum pixel value measured
 * @return  Distance in millimeters on success, UINT16_MAX otherwise
 */
uint16_t logic_calcDist(uint16_t *aquisitionData, uint16_t minVal, uint16_t maxVal, uint16_t maxIndex){
	uint32_t weightedSum = 0;
	uint32_t sum = 0;
	double cog = 0.0;

	// no laser peak detected => error
	if(maxVal < (minVal + MIN_PEAK_HEIGHT)){
		return UINT16_MAX;
	}

	// calculate the center of gravity of the beam
	const uint16_t threshold = (3*minVal + 7*maxVal)/10;
	const uint16_t startIndex = maxIndex > INDEX_THRESHOLD ? maxIndex - INDEX_THRESHOLD : 0;
	const uint16_t endIndex = maxIndex < (NUM_OF_PIX-INDEX_THRESHOLD) ? maxIndex + INDEX_THRESHOLD : NUM_OF_PIX;

	for (uint16_t i = startIndex; i < endIndex; i++) {
		if(aquisitionData[i] >= threshold){
			weightedSum += i*aquisitionData[i];
			sum += aquisitionData[i];
		}
	}
	cog = (double)weightedSum/sum;

	// center of gravity out of range => error
	if(cog < MIN_COG || cog > MAX_COG){
		return UINT16_MAX;
	}

	// calibrated values
	const double A = 8.87605837e-26;
	const double B = -3.82858002e-22;
	const double C = 6.83842202e-19;
	const double D = -6.43172397e-16;
	const double E = 3.30697798e-13;
	const double F = -8.01128756e-11;
	const double G = -6.84663153e-10;
	const double H = 5.21351710e-06;
	const double I = -9.14387821e-04;
	const double J = 2.98917936e-01;
	const double K = 2.51240144e+02;

	// calculate the distance with a calibrated polynomial
	return (uint16_t)(((((((((((A*cog+B)*cog+C)*cog+D)*cog+E)*cog+F)*cog+G)*cog+H)*cog+I)*cog+J)*cog+K)+0.5);
}

/**
 * @brief   Calculates a moving average of the last values
 * @param   new_distance: Newest calculated value
 * @return  Average distance in millimeters on success, UINT16_MAX otherwise
 */
uint16_t logic_movingAverage(uint16_t new_distance)
{
	static uint16_t distBuffer[BUFFER_SIZE] = {0};
	static uint8_t distIndex = 0;
	static uint8_t ctr = 0;
	uint32_t sum = 0;

	// error => reset the buffer
	if(new_distance == UINT16_MAX){
		ctr = 0;
		distIndex = 0;
		return UINT16_MAX;
	}

	// store the new calculated distance
	distBuffer[distIndex] = new_distance;
	distIndex = (distIndex + 1) % BUFFER_SIZE;

	// ensure that this although works if the buffer isn't full
	if (ctr < BUFFER_SIZE) ctr++;

	// calculate the moving average
	for (uint8_t i = 0; i < ctr; i++) {
		sum += distBuffer[i];
	}
	return (uint16_t)(sum/ctr);
}

/**
 * @brief	Sends data over the UART interfaces
 * @param   data: Pointer to the data buffer or string
 * @param   length Number of bytes to send
 */
void logic_writeData(const char *data, int length, bool fastMode)
{
	if (!fastMode){ // don't send data via bluetooth in fast mode
		HAL_UART_Transmit(&huart1, (uint8_t *)data, length, HAL_MAX_DELAY);
	}
	HAL_UART_Transmit(&huart2, (uint8_t *)data, length, HAL_MAX_DELAY);
}
