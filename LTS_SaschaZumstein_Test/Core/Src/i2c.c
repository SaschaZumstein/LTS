/*
This file contains some function for I2C communication

  * Autor: Gion-Pol Catregn (FHGR)
  * Date: 27.02.2021
  * Version: 1.0
  * Changes:
  * 27.02.2021 V1.0 Initial Version
*/

/********************************************************************************************/
/* Includes                                                                                 */
/********************************************************************************************/
#include <stdio.h>
#include "main.h"
#include "i2c.h"

/********************************************************************************************/
/* Local Variables                                                                          */
/********************************************************************************************/
extern I2C_HandleTypeDef hi2c1;		// Handler for the I2C communication
HAL_StatusTypeDef result;	// Variable for the HAL status
uint8_t i2cBuf[32];			// I2C Buffer Array

/********************************************************************************************/
 /* Functions                                                                               */
/********************************************************************************************/
// This Function reads one register over I2C
// Parameters: Device address, Register to read
// Return: The byte stored in the register
HAL_StatusTypeDef I2C_Reset_epc901(void)
{
	// Store the data to the buffer
	i2cBuf[0] = 0x06;
	return HAL_I2C_Master_Transmit(&hi2c1, 0x00, i2cBuf, 1, HAL_MAX_DELAY);// Write the buffer over i2c
}

// This Function reads one register over I2C
// Parameters: Device address, Register to read
// Return: The byte stored in the register
uint8_t I2C_Read_Register(uint8_t address, uint8_t reg)
{
	uint8_t buf[1]; // Buffer containing the data

	// Read Data from a Register
	result = HAL_I2C_Mem_Read(&hi2c1, address, reg, I2C_MEMADD_SIZE_8BIT , buf, 1, HAL_MAX_DELAY);
	if ( result != HAL_OK )
	{
		return 0;
	}
	return buf[0]; // return the read register
}

// This Function writes 1byte of data over I2C
// Parameters: Device address, Register to write, data of the byte
// Return: None
void I2C_Write_Register(uint8_t address, uint8_t reg, uint8_t data)
{
	// Store the register and the data to the buffer
	i2cBuf[0] = reg;
	i2cBuf[1] = data;
	HAL_I2C_Master_Transmit(&hi2c1, address, i2cBuf, 2, HAL_MAX_DELAY);// Write the buffer over i2c
}
