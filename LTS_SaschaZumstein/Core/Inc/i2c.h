/*
 * i2c.h
 *
 * Created on: Apr 28, 2020
 * Author: Gion-Pol Catregn (FHGR)
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

/********************************************************************************************/
/* Global Variables                                                                         */
/********************************************************************************************/

/********************************************************************************************/
/* Functions                                                                                */
/********************************************************************************************/
extern void I2C_Scanner(void);
extern void I2C_Reset_epc901(void);
extern uint8_t I2C_Read_Register(uint8_t address, uint8_t reg);
extern void I2C_Write_Register(uint8_t address, uint8_t reg, uint8_t data);
void I2C_Read_FIFO(uint8_t address, uint8_t reg, uint16_t size);

extern uint8_t data;

#endif /* INC_I2C_H_ */
