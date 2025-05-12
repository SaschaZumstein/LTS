/**
 * @file	i2c.c
 * @brief	Implementation of I2C communication
 *
 * @author	Gion-Pol Catregn (FHGR)
 * @date	Last modified: 27.02.2021
 * @version	1.0
 */

/*------------------------------------------------------------------------------------------*/
/* Includes                                                                                 */
/*------------------------------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include <stdio.h>

/*------------------------------------------------------------------------------------------*/
/* External Handles                                                                         */
/*------------------------------------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

/*------------------------------------------------------------------------------------------*/
/* Functions                                                                                */
/*------------------------------------------------------------------------------------------*/
/**
 * @brief	Reset the epc901 sensor via I2C
 * @return 	HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef I2C_Reset_epc901(void)
{
	uint8_t i2cBuf[32]; // I2C Buffer Array
	i2cBuf[0] = 0x06;
	return HAL_I2C_Master_Transmit(&hi2c1, 0x00, i2cBuf, 1, HAL_MAX_DELAY);// Write the buffer over i2c
}

/**
 * @brief 	Read a byte from a register over I2C.
 * @param 	address: I2C device address.
 * @param 	reg: Register to read from.
 * @return 	The read byte value.
 */
uint8_t I2C_Read_Register(uint8_t address, uint8_t reg)
{
	uint8_t buf[1]; // Buffer containing the data
	// Read Data from a Register
	if (HAL_I2C_Mem_Read(&hi2c1, address, reg, I2C_MEMADD_SIZE_8BIT , buf, 1, HAL_MAX_DELAY) != HAL_OK )
	{
		return 0;
	}
	return buf[0]; // return the read register
}

/**
 * @brief 	Write a byte to a register over I2C.
 * @param 	address: I2C device address.
 * @param 	reg: Register to write to.
 * @param 	data: Byte to write.
 */
void I2C_Write_Register(uint8_t address, uint8_t reg, uint8_t data)
{
	uint8_t i2cBuf[32]; // I2C Buffer Array
	// Store the register and the data to the buffer
	i2cBuf[0] = reg;
	i2cBuf[1] = data;
	HAL_I2C_Master_Transmit(&hi2c1, address, i2cBuf, 2, HAL_MAX_DELAY);// Write the buffer over i2c
}
