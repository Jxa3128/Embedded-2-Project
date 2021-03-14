/*
 * resistance.c
 *
 *  Created on: Mar 13, 2021
 *      Author: jorgejuarez
 */
#include "measurments.h"
#include "gpio.h"

void initResistance()
{
    // Enable clocks
    enablePort(PORTA);
    enablePort(PORTE);
    _delay_cycles(3);

    //select left side
    selectPinPushPullOutput(MEASURE_LR); //pa2
    selectPinPushPullOutput(MEASURE_C); //pe2

    //select right side
    selectPinPushPullOutput(INTEGRATE);
    selectPinPushPullOutput(HIGHSIDE);
    selectPinPushPullOutput(LOWSIDE);

    //now we have to initialize the Analog Comparator
    selectPinAnalogInput(Analog); //pe4 - AFSEL_R,DEN_R and AMSEL_R

    /*
     3:
     In the GPIO module, enable the GPIO port/pin associated with the input signals as GPIO inputs.
     To determine which GPIO to configure, see Table 23-4 on page 1344.

     C0+ 14 Analog Analog comparator 0 positive input.
     C0- 13 Analog Analog comparator 0 negative input.
     */
    selectPinDigitalInput(PORTE, 13);
    selectPinDigitalInput(PORTE, 14);

    /*
     4.
     Configure the PMCn fields in the GPIOPCTL register to assign the analog comparator output
     signals to the appropriate pins (see page 688 and Table 23-5 on page 1351).
     */

    // N/A since we want to output it to the Uart0 ?????
    /*
     5. Configure the internal voltage reference to 1.65 V by writing the ACREFCTL register with the
     value 0x0000.030C.

     */
    COMP_ACREFCTL_R |= 0xF; //2.469 is the highest we want
}

