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

#define CLOCK_RATE 40e6
#define V_REF 2.469

void initMeasure();
uint32_t measureResistance();
void disablePins();
void testBoard();
void reIniciar();
uint32_t measureCapacitance();
uint32_t measureInductance();
double measureAuto();
double measureEsr();
void initAdc();
void setAdcMux(uint8_t input);
double getVoltage(); //esr
uint32_t readAdc();
float getFullVoltage(); //measure up to 3.3V voltage for source
#endif /* MEASURMENTS_H_ */
