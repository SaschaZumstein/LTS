/*
 * epc901.h
 *
 *  Created on: 22.02.2021
 *      Author: Gion-Pol Catregn (FHGR)
 */

#ifndef CCD_EPC_H_
#define CCD_EPC_H_

/* Private function prototypes -----------------------------------------------*/
uint8_t epc901_init();
HAL_StatusTypeDef epc901_getData(uint16_t shutterTime, uint16_t *aquisitionData);
uint16_t epc901_calcDist(uint16_t *aquisitionData);
void usDelay(uint16_t delayTime_us);

#endif /* CCD_EPC_H_ */
