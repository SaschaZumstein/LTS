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
HAL_StatusTypeDef epc901_init() {
	// Contains value of the read register
	uint8_t readByte = 0;

	if(I2C_Reset_epc901() != HAL_OK){
		return HAL_ERROR;
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
		return HAL_ERROR;
	}

	// Use the programmed configuration bits and not the pins
	I2C_Write_Register(EPC901_I2C_ADDRESS, MISC_CONF, 0x19);
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, MISC_CONF);

	// Set the read Direction
	I2C_Write_Register(EPC901_I2C_ADDRESS, ACQ_TX_CONF_I2C, 0x15);
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, ACQ_TX_CONF_I2C);

	return HAL_OK;
}

// Read the data of the epc901 ccd array
HAL_StatusTypeDef epc901_getData(uint16_t shutterTime, uint16_t *aquisitionData, uint16_t *minVal, uint16_t *maxVal)
{
	// reset min, max and mean value
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
	    if(HAL_ADC_Start(&hadc1) != HAL_OK){
	    	return HAL_ERROR;
	    }
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

void epc901_adjustShutterTime(uint16_t *shutterTime, uint16_t minVal, uint16_t maxVal)
{
	const uint16_t shutterList[] = {10, 20, 50, 100, 200, 500, 1000};
	static uint16_t shutterIndex = 3;

	const uint16_t MIN_BASELINE_PEAK = 40<<4;
	const uint16_t MAX_BASELINE_PEAK = 50<<4;
	const uint16_t MIN_BASELINE_NO_PEAK = 95<<4;
	const uint16_t MAX_BASELINE_NO_PEAK = 105<<4;
	const uint16_t MIN_PEAK = 80<<4;
	const uint16_t MAX_PEAK = 120<<4;
	const uint16_t MIN_PEAK_HEIGHT = 15<<4;
	const uint16_t MIN_SHUTTER = 0;
	const uint16_t MAX_SHUTTER = 6;

	// no peak detected => bring baseline to ca. 100
	if(maxVal < minVal + MIN_PEAK_HEIGHT) {
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

void usDelay(uint16_t delayTime_us)
{
	__HAL_TIM_SET_COUNTER(&htim9,0);
	while (__HAL_TIM_GET_COUNTER(&htim9) < (delayTime_us));
}

