/*
 * resistance.c
 *
 *  Created on: Mar 13, 2021
 *      Author: jorgejuarez
 */
#include "measurments.h"
#include "gpio.h"
#include "wait.h"

//define
//left side of the board
#define MEASURE_LR PORTA,2 //PA2
#define MEASURE_C PORTE,2 //PE2
#define ADC PORTE,4 //analog PE4(ADC)
#define AC PORTC,7 //PC7(AC)

//right side
#define INTEGRATE PORTE,5 //PB5
#define HIGHSIDE PORTA,3 //PA3
#define LOWSIDE PORTA,7 //PA7

void initMeasure()
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
    selectPinAnalogInput(AC); //pe4 - AFSEL_R,DEN_R and AMSEL_R

    //enable a "narrow"/wide timer but it has to be 32-bit
    // TIMER Configure
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R0;                // turn-on timer
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;   // turn-off counter before reconfiguring
    WTIMER0_CFG_R = 4;                   // configure as 32-bit counter (A only)
    WTIMER0_TAMR_R = TIMER_TAMR_TACMR | TIMER_TAMR_TAMR_CAP | TIMER_TAMR_TACDIR; // configure for edge time mode, count up
    WTIMER0_CTL_R = TIMER_CTL_TAEVENT_POS; // measure time from positive edge to positive edge
    WTIMER0_TAV_R = 0;                          // zero counter for first period

    //1: enable clocks
    SYSCTL_RCGCACMP_R |= 1;
    /*
     5. Configure the internal voltage reference to 1.65 V by writing the ACREFCTL register with the
     value 0x0000.030C.

     */
    COMP_ACREFCTL_R |= 0xF | (1 << 9) | (0 << 8); //(0<<8) RNG=0, (1<<9) = EN, 0xF = 2.469
    COMP_ACCTL0_R |= (2 << 9) | (1 << 1); //ASRCP && (1<<1) CINV we want to invert

    waitMicrosecond(10);

}

