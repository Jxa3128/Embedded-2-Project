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
void reIniciar();
uint32_t measureCapacitance();
uint32_t measureInductance();
#endif /* MEASURMENTS_H_ */
