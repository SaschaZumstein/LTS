/**
 * @file	epc901.h
 * @brief	Header file for epc901 line sensor
 *
 * @author	Gion-Pol Catregn (FHGR)
 * @author	Sascha Zumstein (FHGR)
 * @date	Last modified: 12.05.2025
 * @version 1.0
 */

#ifndef EPC_901_H_
#define EPC_901_H_

/*------------------------------------------------------------------------------------------*/
/* Function prototypes                                                                      */
/*------------------------------------------------------------------------------------------*/
/**
 * @brief 	Initializes the epc901 sensor via I2C
 * @return 	HAL_OK on success, HAL_ERROR otherwise
 */
HAL_StatusTypeDef epc901_init();

/**
 * @brief 	Read out the data from the epc901 sensor
 * @param 	shutterTime: Exposure time in microseconds
 * @param 	acquisitionData: Pointer to buffer for raw pixel values (1024 values)
 * @param 	minVal: Pointer to return minimum pixel value
 * @param 	maxVal: Pointer to return maximum pixel value
 * @return 	HAL_OK on success, HAL_ERROR otherwise
 */
HAL_StatusTypeDef epc901_getData(uint16_t shutterTime, uint16_t *aquisitionData, uint16_t *minVal, uint16_t *maxVal, uint16_t *maxIndex);

/**
 * @brief 	Microsecond delay function
 * @param 	delayTime_us: Delay time in microseconds
 */
void usDelay(uint16_t delayTime_us);

#endif /* EPC_901_H_ */
