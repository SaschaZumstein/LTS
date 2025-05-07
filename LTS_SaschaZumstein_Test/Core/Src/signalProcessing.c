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
#include "signalProcessing.h"

/********************************************************************************************/
/* Defines                                                                       */
/********************************************************************************************/
#define NUM_OF_PIX 1024

/********************************************************************************************/
/* Functions                                                                        */
/********************************************************************************************/
uint16_t sigProc_calcDist(uint16_t *aquisitionData, uint16_t minMaxMiddle, uint16_t maxVal, uint16_t baseline){
	uint32_t weightedSum = 0;
	uint32_t sum = 0;
	double cog = 0.0;
	uint16_t distance = 0;

	const uint16_t MIN_PEAK_HEIGHT = 15<<4;
	const uint16_t MIN_DISTANCE = 260;
	const uint16_t MAX_DISTANCE = 1200;

	// fit parameter
	const double A = -3.11767119e-25;
	const double B = 1.49183346e-21;
	const double C = -3.02357839e-18;
	const double D = 3.39036248e-15;
	const double E = -2.30213278e-12;
	const double F = 9.74279304e-10;
	const double G = -2.54731085e-07;
	const double H = 3.96967745e-05;
	const double I = -3.18417563e-03;
	const double J = 3.50999221e-01;
	const double K = 2.54416652e+02;

	// no laser peak detected => error
	if(maxVal < baseline + MIN_PEAK_HEIGHT){
		return UINT16_MAX;
	}

	// calculate the center of gravity of the beam
	for (int i = 0; i < NUM_OF_PIX; i++) {
		if(aquisitionData[i] >= minMaxMiddle){
			weightedSum += i*aquisitionData[i];
			sum += aquisitionData[i];
		}
	}
	cog = (double)weightedSum/sum;

	// calculate the distance with a calibrated polynomial
	distance = (uint16_t)((((((((((A*cog+B)*cog+C)*cog+D)*cog+E)*cog+F)*cog+G)*cog+H)*cog+I)*cog+J)*cog+K);
	// distance to high or to low
	if(distance < MIN_DISTANCE || distance > MAX_DISTANCE){
		return UINT16_MAX;
	}
	return distance;
}
