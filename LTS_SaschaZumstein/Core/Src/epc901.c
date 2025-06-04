/**
 * @file 	epc901.c
 * @brief 	Driver implementation for epc901 line sensor
 *
 * @author 	Gion-Pol Catregn (FHGR)
 * @author 	Sascha Zumstein (FHGR)
 * @date 	Last modified: 12.05.2025
 * @version 1.0
 */

/*------------------------------------------------------------------------------------------*/
/* Includes                                                                                 */
/*------------------------------------------------------------------------------------------*/
#include "main.h"
#include "epc901.h"
#include "i2c.h"
#include <stdio.h>

/*------------------------------------------------------------------------------------------*/
/* Defines                                                                                  */
/*------------------------------------------------------------------------------------------*/
#define EPC901_I2C_ADDRESS 0x15<<1 	// I2C address of epc901
#define NUM_OF_PIX 1024 			// Number of pixels in EPC901 sensor

/*------------------------------------------------------------------------------------------*/
/* Typedefs                                                                                 */
/*------------------------------------------------------------------------------------------*/
/**
 * @brief 	Register map for EPC901 configuration via I2C
 */
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

/*------------------------------------------------------------------------------------------*/
/* External Handles                                                                         */
/*------------------------------------------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim9;

/*------------------------------------------------------------------------------------------*/
/* Functions                                                                                */
/*------------------------------------------------------------------------------------------*/
/**
 * @brief 	Initializes the epc901 sensor via I2C
 * @return 	HAL_OK on success, HAL_ERROR otherwise
 */
HAL_StatusTypeDef epc901_init() {
	uint8_t readByte = 0; // Contains value of the read register

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

/**
 * @brief 	Read out the data from the epc901 sensor
 * @param 	shutterTime: Exposure time in microseconds
 * @param 	acquisitionData: Pointer to buffer for raw pixel values (1024 values)
 * @param 	minVal: Pointer to return minimum pixel value
 * @param 	maxVal: Pointer to return maximum pixel value
 * @return 	HAL_OK on success, HAL_ERROR otherwise
 */
HAL_StatusTypeDef epc901_getData(uint16_t shutterTime, uint16_t *aquisitionData, uint16_t *minVal, uint16_t *maxVal, uint16_t *maxIndex)
{
	// reset min and max value and index
	(*minVal) = UINT16_MAX;
	(*maxVal) = 0;
	(*maxIndex) = 0;

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
	for (uint16_t i = 0; i < NUM_OF_PIX; i++) {
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
			(*maxIndex) = i;
		}
	}
	return HAL_OK;
}

/**
 * @brief 	Microsecond delay function
 * @param 	delayTime_us: Delay time in microseconds
 */
void usDelay(uint16_t delayTime_us)
{
	__HAL_TIM_SET_COUNTER(&htim9,0);
	while (__HAL_TIM_GET_COUNTER(&htim9) < (delayTime_us));
}

