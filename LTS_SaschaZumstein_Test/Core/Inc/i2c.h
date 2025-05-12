/**
 * @file	i2c.h
 * @brief	Header file for I2C communication
 *
 * @author	Gion-Pol Catregn (FHGR)
 * @date	Last modified: 27.02.2021
 * @version	1.0
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_
/*------------------------------------------------------------------------------------------*/
/* Global Variables                                                                    */
/*------------------------------------------------------------------------------------------*/
extern uint8_t data;

/*------------------------------------------------------------------------------------------*/
/* Function prototypes                                                                      */
/*------------------------------------------------------------------------------------------*/
/**
 * @brief	Reset the epc901 sensor via I2C
 * @return 	HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef I2C_Reset_epc901(void);

/**
 * @brief 	Read a byte from a register over I2C.
 * @param 	address: I2C device address.
 * @param 	reg: Register to read from.
 * @return 	The read byte value.
 */
uint8_t I2C_Read_Register(uint8_t address, uint8_t reg);

/**
 * @brief 	Write a byte to a register over I2C.
 * @param 	address: I2C device address.
 * @param 	reg: Register to write to.
 * @param 	data: Byte to write.
 */
void I2C_Write_Register(uint8_t address, uint8_t reg, uint8_t data);

#endif /* INC_I2C_H_ */
