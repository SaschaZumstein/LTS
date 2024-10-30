/*
 * epc901.h
 *
 *  Created on: 22.02.2021
 *      Author: Gion-Pol Catregn (FHGR)
 */

#ifndef CCD_EPC_H_
#define CCD_EPC_H_

/* Private function prototypes -----------------------------------------------*/
void epc901_init();
void epc901_getData(int shutterTime);
void usDelay(uint16_t delayTime_us);

#endif /* CCD_EPC_H_ */
