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

// This Function scans the I2c bus for connected devices
// Parameters: None
// Return: None
void I2C_Scanner(void)
{
	// Output to the UART
	printf("Scanning I2C bus:\r\n");
	printf("---------------------\r\n\n");

	// Scan all addresses
   	uint8_t i;
   	for (i=1; i<128; i++)
   	{
   		// Check if there is a response on the corresponding address
   		result = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 2, 2);
   		if (result != HAL_OK) // HAL_ERROR or HAL_BUSY or HAL_TIMEOUT
   		{
   			//printf(".\r"); // No ACK received at that address
   		}
   		if (result == HAL_OK)
   		{
   			printf("Device found at address : 0x%X\r\n", i); // Received an ACK at that address
   		}
   		HAL_Delay(5); // Delay of 5ms
   	}
   	printf("\r\n");
}

// This Function reads one register over I2C
// Parameters: Device address, Register to read
// Return: The byte stored in the register
void I2C_Reset_epc901(void)
{
	// Store the data to the buffer
	i2cBuf[0] = 0x06;
	result = HAL_I2C_Master_Transmit(&hi2c1, 0x00, i2cBuf, 1, HAL_MAX_DELAY);// Write the buffer over i2c
	if ( result != HAL_OK )
	{
		printf("I2C ERROR!\r\n");
	}
	else
	{
		printf("epc901 is reset!\r\n");
	}

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
		printf("I2C ERROR!\r\n");
	}
	else
	{
		//printf("Register read done!\r\n");
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
	result = HAL_I2C_Master_Transmit(&hi2c1, address, i2cBuf, 2, HAL_MAX_DELAY);// Write the buffer over i2c
	if ( result != HAL_OK )
	{
		printf("I2C ERROR!\r\n");
	}
	else
	{
		//printf("Register write done!\r\n");
	}

}

// This Function reads a burst of data over I2C
// Parameters: Device address, Register to read, Size of bytes to read
// Return: Data is stored in a global array
void I2C_Read_FIFO(uint8_t address, uint8_t reg, uint16_t size)
{
	//fifoData[0] = reg; // Store the register to the array
	//result = HAL_I2C_Master_Transmit(&hi2c1, address, fifoData, 1, HAL_MAX_DELAY); // Write the register address over i2c
	if ( result != HAL_OK )
	{

	}
	else
	{
		//result = HAL_I2C_Master_Receive(&hi2c1, address, fifoData, size, HAL_MAX_DELAY); // Store the bytes to the global array
		if ( result != HAL_OK ) {

		}
		else
		{
			//printf("FIFO read done!\r\n");
		}
	}
}

