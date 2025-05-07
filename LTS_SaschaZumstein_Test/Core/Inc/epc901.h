/*
 * epc901.h
 *
 *  Created on: 22.02.2021
 *      Author: Gion-Pol Catregn (FHGR)
 */

#ifndef EPC_901_H_
#define EPC_901_H_

/* Private function prototypes -----------------------------------------------*/
HAL_StatusTypeDef epc901_init();
HAL_StatusTypeDef epc901_getData(uint16_t shutterTime, uint16_t *aquisitionData, uint16_t *minVal, uint16_t *maxVal, uint16_t *meanVal);
void epc901_adjustShutterTime(uint16_t *shutterTime, uint16_t maxVal, uint16_t baseline);
void usDelay(uint16_t delayTime_us);

#endif /* EPC_901_H_ */
