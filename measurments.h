/*
 * resistance.h
 *
 *  Created on: Mar 13, 2021
 *      Author: jorgejuarez
 */
#include "tm4c123gh6pm.h"
#ifndef MEASURMENTS_H_
#define MEASURMENTS_H_


//define
//left side of the board
#define MEASURE_LR PORTA,2 //PA2
#define MEASURE_C PORTE,2 //PE2
#define Analog PORTE,4 //analog PE4

 //right side
#define INTEGRATE PORTE,5 //PB5
#define HIGHSIDE PORTA,3 //PA3
#define LOWSIDE PORTA,7 //PA7


#endif /* MEASURMENTS_H_ */
