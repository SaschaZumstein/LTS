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
#include "conn.h"
#include <math.h>

/********************************************************************************************/
/* Defines                                                                       */
/********************************************************************************************/
#define NUM_OF_PIX 1024

/********************************************************************************************/
/* Type Definitions                                                                        */
/********************************************************************************************/
extern UART_HandleTypeDef huart2;
/********************************************************************************************/
/* Functions                                                                        */
/********************************************************************************************/
uint16_t sigProc_calcDist(uint16_t *aquisitionData, uint16_t minMaxMiddle){
	uint32_t weightedSum = 0;
	uint32_t sum = 0;
	double cog = 0.0;

	// fit parameter
	double a = -3.11767119e-25;
	double b = 1.49183346e-21;
	double c = -3.02357839e-18;
	double d = 3.39036248e-15;
	double e = -2.30213278e-12;
	double f = 9.74279304e-10;
	double g = -2.54731085e-07;
	double h = 3.96967745e-05;
	double i = -3.18417563e-03;
	double j = 3.50999221e-01;
	double k = 2.54416652e+02;

	for (int i = 0; i < NUM_OF_PIX; i++) {
		// extract the laser beam from noise
		if(aquisitionData[i] < minMaxMiddle){
			aquisitionData[i] = 0;
		}

		// calculate the center of gravity of the beam
		weightedSum += i*aquisitionData[i];
		sum += aquisitionData[i];
	}
	cog = (double)weightedSum/sum;

	return (uint16_t)(a*pow(cog,10)+b*pow(cog,9)+c*pow(cog,8)+d*pow(cog,7)+e*pow(cog,6)+f*pow(cog,5)+g*pow(cog,4)+h*pow(cog,3)+i*pow(cog,2)+j*cog+k);
}
