/*
This file contains the initialization and readout of the epc901 ccd array

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
#include <stdio.h>

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

void epc901_init() {

	// Contains value of the read register
	uint8_t readByte = 0;

	// Output to the UART
	printf("Initialization of the epc901\r\n");
	printf("---------------------------------\r\n");

	// SW reset of the epc901
	printf("\nReset the epc901:\r\n");
	I2C_Reset_epc901();

    // Read some Registers
	printf("\nRead some registers:\r\n");

	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, CHIP_REV_NO_REG);
	printf("CHIP_REV_NO_REG	 Register = 0x%x\r\n",readByte);

	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, ACQ_TX_CONF_I2C);
	printf("ACQ_TX_CONF_I2C	 Register = 0x%x\r\n",readByte);

	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, MISC_CONF);
	printf("MISC_CONF Register = 0x%x\r\n",readByte);

	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, BW_VIDEO_CONF_I2C);
	printf("BW_VIDEO_CONF_I2C = 0x%x\r\n",readByte);

	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, FORCE_ANA_CTRL_SIGS);
	printf("FORCE_ANA_CTRL_SIGS = 0x%x\r\n",readByte);

    // Write some Registers
	printf("\nWrite some registers:\r\n");

	// enter the test mode
	I2C_Write_Register(EPC901_I2C_ADDRESS, ANA_TEST_MODE_EN_0, 0x4A);
	I2C_Write_Register(EPC901_I2C_ADDRESS, ANA_TEST_MODE_EN_1, 0x66);

	// turn off charge pump
	I2C_Write_Register(EPC901_I2C_ADDRESS, FORCE_ANA_CTRL_SIGS, 0x03);

	// Read the new value of the register
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, FORCE_ANA_CTRL_SIGS);
	printf("FORCE_ANA_CTRL_SIGS = 0x%x\r\n",readByte);

	// Check if charge pump is off
	if (readByte == 0x03)
	{
		printf("Charge pump is switched off!!:\r\n");
	}
	else
	{
		printf("Error switching off the charge pump!!:\r\n");
	}

	// Use the programmed configuration bits and not the pins
	I2C_Write_Register(EPC901_I2C_ADDRESS, MISC_CONF, 0x19);
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, MISC_CONF);
	printf("MISC_CONF Register = 0x%x\r\n",readByte);

	// Set the read Direction
	I2C_Write_Register(EPC901_I2C_ADDRESS, ACQ_TX_CONF_I2C, 0x15);
	readByte = I2C_Read_Register(EPC901_I2C_ADDRESS, ACQ_TX_CONF_I2C);
	printf("ACQ_TX_CONF_I2C	 Register = 0x%x\r\n",readByte);

	printf("\nInitialization of the epc901 done.\r\n\n");
}


// Read the data of the epc901 ccd array
void epc901_getData(int shutterTime)
{
	uint16_t aquisitionData[NUM_OF_PIX] = {0};
	uint8_t buffer[1];

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
	while (!HAL_GPIO_ReadPin(DATA_RDY_GPIO_Port, DATA_RDY_Pin))
	{
	}
	//printf("Data_RDY is set!:\r\n"); // Only for debugging.


	// first pulse on READ to start conversion
	HAL_GPIO_WritePin(READ_GPIO_Port, READ_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(READ_GPIO_Port, READ_Pin, GPIO_PIN_RESET);
	usDelay(1); // delay TCD


	// Preload the pipeline
	for (int i = 0; i < 3; i++) {
		HAL_GPIO_WritePin(READ_GPIO_Port, READ_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(READ_GPIO_Port, READ_Pin, GPIO_PIN_RESET);
	}
	
	// Start of Frame
	printf("START DATA\r\n");

	// Readout the data
	for (int i = 0; i < NUM_OF_PIX; i++) {
		HAL_GPIO_WritePin(READ_GPIO_Port, READ_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(READ_GPIO_Port, READ_Pin, GPIO_PIN_RESET);

	    // Get ADC value
	    HAL_ADC_Start(&hadc1);
	    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	    aquisitionData[i] = HAL_ADC_GetValue(&hadc1);
		
		// Send the Data to the Serial interface
	    //printf("%d: %d\r\n", i, aquisitionData[i] >> 4);
		// Faster with the HAL library than with the printf funktion
	    buffer[0] = aquisitionData[i] >> 4;
	    HAL_UART_Transmit(&huart2, buffer , 1, HAL_MAX_DELAY);
	}
	// End of Frame
	printf("\r\nEND DATA\r\n");
}

void usDelay(uint16_t delayTime_us)
{
	__HAL_TIM_SET_COUNTER(&htim9,0);
	while (__HAL_TIM_GET_COUNTER(&htim9) < (delayTime_us));
}

