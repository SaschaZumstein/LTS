/*This file contains the initialization and readout of the epc901 ccd array

  * Author: Gion-Pol Catregn (FHGR)
  * Date: 25.02.2021
  * Version: 1.0
  * Changes:
  * 25.02.2021 V1.0 Initial Version
*/


/********************************************************************************************/
/* Includes                                                                                 */
/********************************************************************************************/

#include "main.h"
#include "epc901.h"
#include "i2c.h"
#include "conn.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/********************************************************************************************/
/* Defines                                                                       */
/********************************************************************************************/
#define EPC901_I2C_ADDRESS 0x15<<1
#define NUM_OF_PIX 1024

/********************************************************************************************/
/* Type Definitions                                                                        */
/********************************************************************************************/
typedef enum
{
	ACQ_TX_CONF_I2C			= 0x00,
	BW_VIDEO_CONF_I2C		= 0x01,
	MISC_CONF				= 0x02,
	ANA_TEST_MODE_EN_0 		= 0xD0,
	ANA_TEST_MODE_EN_1 		= 0xD1,
	FORCE_ANA_CTRL_SIGS 	= 0xD6,
	CHIP_REV_NO_REG			= 0xFF
}CCD_CONFIG_REG;

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim9;

/********************************************************************************************/
/* Functions                                                                        */
/********************************************************************************************/
// Initialization of the epc901 ccd Array
uint8_t epc901_init() {
	// Contains value of the read register
	uint8_t readByte = 0;

	if(I2C_Reset_epc901() != HAL_OK){
		return 0;
	}

    // Read some Registers
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, CHIP_REV_NO_REG);
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, ACQ_TX_CONF_I2C);
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, MISC_CONF);
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, BW_VIDEO_CONF_I2C);
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, FORCE_ANA_CTRL_SIGS);

    // Write some Registers and enter the test mode
	I2C_Write_Register(EPC901_I2C_ADDRESS, ANA_TEST_MODE_EN_0, 0x4A);
	I2C_Write_Register(EPC901_I2C_ADDRESS, ANA_TEST_MODE_EN_1, 0x66);
	// turn off charge pump
	I2C_Write_Register(EPC901_I2C_ADDRESS, FORCE_ANA_CTRL_SIGS, 0x03);
	// Read the new value of the register
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, FORCE_ANA_CTRL_SIGS);
	// Check if charge pump is off
	if (readByte != 0x03)
	{
		return 0;
	}

	// Use the programmed configuration bits and not the pins
	I2C_Write_Register(EPC901_I2C_ADDRESS, MISC_CONF, 0x19);
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, MISC_CONF);

	// Set the read Direction
	I2C_Write_Register(EPC901_I2C_ADDRESS, ACQ_TX_CONF_I2C, 0x15);
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, ACQ_TX_CONF_I2C);

	return 1;
}


// Read the data of the epc901 ccd array
HAL_StatusTypeDef epc901_getData(uint16_t shutterTime, uint16_t *aquisitionData, uint16_t *minVal, uint16_t *maxVal)
{
	// reset min and max value
	(*minVal) = UINT16_MAX;
	(*maxVal) = 0;

	if (I2C_Read_Register(EPC901_I2C_ADDRESS, CHIP_REV_NO_REG) == 0) {
		return HAL_ERROR;
	}

	// Clear pixels from electrons
	for(int i = 0; i < 3; i++) {
		HAL_GPIO_WritePin(CLR_PIX_GPIO_Port, CLR_PIX_Pin, GPIO_PIN_SET);
		usDelay(1);
		HAL_GPIO_WritePin(CLR_PIX_GPIO_Port, CLR_PIX_Pin, GPIO_PIN_RESET);
		usDelay(1);
	}

	// Open shutter
	HAL_GPIO_WritePin(SHUTTER_GPIO_Port, SHUTTER_Pin, GPIO_PIN_SET);
	usDelay(shutterTime);
	HAL_GPIO_WritePin(SHUTTER_GPIO_Port, SHUTTER_Pin, GPIO_PIN_RESET);

	// Wait until data is ready
	while (!HAL_GPIO_ReadPin(DATA_RDY_GPIO_Port, DATA_RDY_Pin)){}

	// first pulse on READ to start conversion
	HAL_GPIO_WritePin(READ_GPIO_Port, READ_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(READ_GPIO_Port, READ_Pin, GPIO_PIN_RESET);
	usDelay(1); // delay TCD

	// Preload the pipeline
	for (int i = 0; i < 3; i++) {
		HAL_GPIO_WritePin(READ_GPIO_Port, READ_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(READ_GPIO_Port, READ_Pin, GPIO_PIN_RESET);
	}
	// Readout the data
	for (int i = 0; i < NUM_OF_PIX; i++) {
		HAL_GPIO_WritePin(READ_GPIO_Port, READ_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(READ_GPIO_Port, READ_Pin, GPIO_PIN_RESET);

	    // Get ADC value
	    HAL_ADC_Start(&hadc1);
	    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	    aquisitionData[i] = HAL_ADC_GetValue(&hadc1);

	    // search for min and max value
		if((*minVal) > aquisitionData[i]){
			(*minVal) = aquisitionData[i];
		}
		if((*maxVal) < aquisitionData[i]){
			(*maxVal) = aquisitionData[i];
		}
	}
	return HAL_OK;
}

uint16_t epc901_calcDist(uint16_t *aquisitionData, uint16_t *minVal, uint16_t *maxVal){
	uint8_t buffer[1];
	uint16_t minMaxMiddle = ((*maxVal)+(*minVal))/2;
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

	// Start of Frame
	conn_writeData("START DATA\r\n", 12);
	for (int i = 0; i < NUM_OF_PIX; i++) {
		// extract the laser beam from noise
		if(aquisitionData[i] < minMaxMiddle){
			aquisitionData[i] = 0;
		}

		// calculate the center of gravity of the beam
		weightedSum += i*aquisitionData[i];
		sum += aquisitionData[i];

		// transmit the data
		buffer[0] = aquisitionData[i] >> 4;
		HAL_UART_Transmit(&huart2, buffer , 1, HAL_MAX_DELAY);
	}
	conn_writeData("\r\nEND DATA\r\n", 12);
	cog = (double)weightedSum/sum;

	return (uint16_t)(a*pow(cog,10)+b*pow(cog,9)+c*pow(cog,8)+d*pow(cog,7)+e*pow(cog,6)+f*pow(cog,5)+g*pow(cog,4)+h*pow(cog,3)+i*pow(cog,2)+j*cog+k);
}

void epc901_regulateShutterTime(uint16_t *shutterTime, uint16_t *maxVal)
{
	if(((*maxVal)>>4) < 110 && (*shutterTime) < 800) {
		(*shutterTime) += 100;
	}
}

void usDelay(uint16_t delayTime_us)
{
	__HAL_TIM_SET_COUNTER(&htim9,0);
	while (__HAL_TIM_GET_COUNTER(&htim9) < (delayTime_us));
}

