/*
 * resistance.h
 *
 *  Created on: Mar 13, 2021
 *      Author: jorgejuarez
 */
#include "tm4c123gh6pm.h"
#include <stdint.h>

#ifndef MEASURMENTS_H_
#define MEASURMENTS_H_

void initMeasure();
uint32_t measureResistance();
void disablePins();
void testBoard();

#endif /* MEASURMENTS_H_ */
