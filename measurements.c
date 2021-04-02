/*
 * resistance.c
 *
 *  Created on: Mar 13, 2021
 *      Author: jorgejuarez
 */
#include "measurments.h"
#include "gpio.h"
#include "wait.h"
#include "uart0.h"
#include "tm4c123gh6pm.h"

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

#define BLUE_LED PORTF,2
#define RED_LED PORTF,1

#define CONST_RES 58
#define CONST_CAP 100e3
#define INDUCT 1
#define ESR_OFFSET 4.7
#define TEST_VALUE (2<<7)

void initMeasure()
{
    // Enable clocks
    enablePort(PORTA);
    enablePort(PORTE);
    enablePort(PORTC);
    _delay_cycles(3);

    //turn blue LED on
    enablePort(PORTF);
    selectPinPushPullOutput(BLUE_LED);
    selectPinPushPullOutput(RED_LED);

    //select left side
    selectPinPushPullOutput(MEASURE_LR); //pa2
    selectPinPushPullOutput(MEASURE_C); //pe2

    //select right side
    selectPinPushPullOutput(INTEGRATE);
    selectPinPushPullOutput(HIGHSIDE);
    selectPinPushPullOutput(LOWSIDE);

    //initalizing the ADC will go here - PE4
    selectPinAnalogInput(ADC);
    initAdc();
    setAdcMux(9);
    //now we have to initialize the Analog Comparator
    selectPinAnalogInput(AC); //PC7 - AFSEL_R,DEN_R and AMSEL_R

    /*
     * TIMER0 --> resistance
     */
    //enable a "narrow"/wide timer but it has to be 32-bit
    // TIMER Configure
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R0;                // turn-on timer
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;   // turn-off counter before reconfiguring
    WTIMER0_CFG_R = 4;                   // configure as 32-bit counter (A only)
    WTIMER0_TAMR_R = TIMER_TAMR_TACMR | TIMER_TAMR_TAMR_CAP | TIMER_TAMR_TACDIR; // configure for edge time mode, count up
    WTIMER0_CTL_R = TIMER_CTL_TAEVENT_POS; // measure time from positive edge to positive edge

    WTIMER0_TAV_R = 0;                          // zero counter for first period
    WTIMER0_TBV_R = 0;

    //1: enable clocks
    SYSCTL_RCGCACMP_R |= 1;
    /* (starts on page 1220)
     5. Configure the internal voltage reference to 1.65 V by writing the ACREFCTL register with the
     value 0x0000.030C.

     */
    //look at line 6621 in header file for specific names
    COMP_ACREFCTL_R |= 0xF | (1 << 9) | (0 << 8); //(0<<8) RNG=0, (1<<9) = EN, 0xF = 2.469
    COMP_ACCTL0_R |= (2 << 9) | (1 << 1); //ASRCP && (1<<1) CINV we want to invert

    //wait 10us
    waitMicrosecond(10);

}

// RESISTANCE ** RESISTANCE ** RESISTANCE
uint32_t measureResistance()
{
    //make function that disables all the pins
    disablePins();
    setPinValue(INTEGRATE, 1);
    setPinValue(LOWSIDE, 1); //discharge //ground both sides of capacitor
    waitMicrosecond(10e5); //wait a reasonable time

    //disable timer
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;

    //reset timer register TAV and TBV
    WTIMER0_TAV_R = 0;
    WTIMER0_TBV_R = 0;

    //initiate measure for resistance
    setPinValue(LOWSIDE, 0);
    setPinValue(MEASURE_LR, 1);
    //turn on timer
    WTIMER0_CTL_R |= TIMER_CTL_TAEN;

    //stay blocking when it is not tripped - same thing as & with 2
    while (!(COMP_ACSTAT0_R & (1 << 1)))
        ; //this is in page 1226, status register

    //make sure it is not counting
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;

    //disable pins once more before returning
    disablePins();
    //do some math and divide to get accurate resistance and then return value
    //uint32_t k = WTIMER0_TAV_R/CONST_RES;
    return (WTIMER0_TAV_R / CONST_RES) + 1;
    //returns close resistance
}

//measure capacitance

uint32_t measureCapacitance()
{
    disablePins();
    setPinValue(LOWSIDE, 1); //discharge
    waitMicrosecond(10e5); //wait a reasonable time

    //disable timer
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;

    //reset timer register TAV and TBV
    WTIMER0_TAV_R = 0;
    WTIMER0_TBV_R = 0;

    WTIMER0_CTL_R |= TIMER_CTL_TAEN;

    setPinValue(MEASURE_C, 1);

    //initiate measuring for capacitance
    setPinValue(LOWSIDE, 0);
    setPinValue(HIGHSIDE, 1);

    //stay blocking when it is not tripped - same thing as & with 2
    while (!(COMP_ACSTAT0_R & (1 << 1)))
        ; //this is in page 1226, status register

    //waitMicrosecond(10e6);
    //make sure it is not counting
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;
    uint32_t temp = WTIMER0_TAV_R;
    //disable pins once more before returning
    disablePins();

    //return (WTIMER0_TAV_R/CONST_CAP);
    return (WTIMER0_TAV_R * 0.000000169334764); //value was found from a reoccuring pattern of 10/555 = .18 * .00000018
    //plus six zeros since we want it in microfarads
}

uint32_t measureInductance()
{
    disablePins();
    setPinValue(LOWSIDE, 1);
    waitMicrosecond(3e6); //wait a reasonable time

    //disable timer
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;

    //reset timer register TAV and TBV (TBV isnt really needed)
    WTIMER0_TAV_R = 0;
    WTIMER0_TBV_R = 0;

    //enable timer
    WTIMER0_CTL_R |= TIMER_CTL_TAEN;

    //turn LR MEASURE_LR on
    setPinValue(MEASURE_LR, 1);

    //stay blocking when it is not tripped - same thing as & with 2
    while (!(COMP_ACSTAT0_R & (1 << 1)))
        ; //this is in page 1226, status register

    //stop counting
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;
    //ground pins once again
    disablePins();

    return (WTIMER0_TAV_R * .628); //voltage will be rising on the 33 -> *.628, 37631
    //ohms resistor to the reference of 2.469

}
double measureEsr()
{
    disablePins();
    setPinValue(LOWSIDE, 1);
    setPinValue(MEASURE_LR, 1);
    waitMicrosecond(5e5);
    double result = 0.0;
    double dut2Voltage = getVoltage();
    //do some math
    result = (33*(3.3-dut2Voltage))/(dut2Voltage)-ESR_OFFSET;
    return result;
}
//get the votlage on DUT2
double getVoltage()
{
    uint32_t DUT2_raw = readAdc();
    double voltage = ((DUT2_raw - 0.5) / 4096.0 * 3.3);
    return voltage;
}
void initAdc()
{
    // Enable clocks
    SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R0;
    _delay_cycles(16);

    // Configure ADC
    ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN3; // disable sample sequencer 3 (SS3) for programming
    ADC0_CC_R = ADC_CC_CS_SYSPLL; // select PLL as the time base (not needed, since default value)
    ADC0_PC_R = ADC_PC_SR_1M;                        // select 1Msps rate
    ADC0_EMUX_R = ADC_EMUX_EM3_PROCESSOR; // select SS3 bit in ADCPSSI as trigger
    ADC0_SSCTL3_R = ADC_SSCTL3_END0;             // mark first sample as the end
    ADC0_ACTSS_R |= ADC_ACTSS_ASEN3;                 // enable SS3 for operation
}
void setAdcMux(uint8_t input)
{
    ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN3; // disable sample sequencer 3 (SS3) for programming
    ADC0_SSMUX3_R = input;                 // Set analog input for single sample
    ADC0_ACTSS_R |= ADC_ACTSS_ASEN3;                 // enable SS3 for operation
}
// Request and read one sample from SS3
uint32_t readAdc()
{
    ADC0_PSSI_R |= ADC_PSSI_SS3;                     // set start bit
    while (ADC0_ACTSS_R & ADC_ACTSS_BUSY)
        ;           // wait until SS3 is not busy
    while (ADC0_SSFSTAT3_R & ADC_SSFSTAT3_EMPTY)
        ;
    return ADC0_SSFIFO3_R;                    // get single result from the FIFO
}
//maybe extra credit, is fo, I will not do it
double measureAuto()
{
    return (double) TEST_VALUE;
}
void disablePins()
{
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 0);

    setPinValue(LOWSIDE, 0);
    setPinValue(MEASURE_LR, 0);
    setPinValue(MEASURE_C, 0);
//    setPinValue(ADC, 0);
//    setPinValue(AC, 0);
    setPinValue(INTEGRATE, 0);
    setPinValue(HIGHSIDE, 0);
}

void reIniciar()
{
    waitMicrosecond(10e4);
    putsUart0("TM4C123 has been reset!\n");
    NVIC_APINT_R = NVIC_APINT_VECTKEY | NVIC_APINT_VECT_RESET;
}

void testBoard()
{
    //RIGHT SIDE

    //MEASURE_LR =0, MEASURE_C = 0
    setPinValue(BLUE_LED, 1);

    setPinValue(MEASURE_LR, 0);
    setPinValue(MEASURE_C, 0);
    waitMicrosecond(1e6);
    //expected output = floating (.9V~1.1V)
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    waitMicrosecond(1e6);
    disablePins();

    //MEASURE_LR =1, MEASURE_C = 0, DUT1 ~ 3.2+V (logical high)
    setPinValue(BLUE_LED, 1);
    setPinValue(MEASURE_LR, 1);
    setPinValue(MEASURE_C, 0);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    disablePins();
    waitMicrosecond(1e6);

    //MEASURE_LR =0, MEASURE_C = 1, output ~ .02V (logical low)
    setPinValue(BLUE_LED, 1);
    setPinValue(MEASURE_LR, 0);
    setPinValue(MEASURE_C, 1);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    disablePins();
    waitMicrosecond(1e6);

    //RIGHT SIDE BOARD

    //Highside = 0, Lowside = 0, Integrate=0, output = floating (.9V~1.1V)
    setPinValue(BLUE_LED, 1);
    setPinValue(LOWSIDE, 0);
    setPinValue(HIGHSIDE, 0);
    setPinValue(INTEGRATE, 0);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    disablePins();
    waitMicrosecond(1e6);

    //Highside = 1, Lowside = 0, Integrate=0, output ~3V-3.15V
    setPinValue(BLUE_LED, 1);
    setPinValue(LOWSIDE, 0);
    setPinValue(HIGHSIDE, 1);
    setPinValue(INTEGRATE, 0);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    disablePins();
    waitMicrosecond(1e6);

    //Highside = 0, Lowside = 1, Integrate=0, output (.2V) logical low
    setPinValue(BLUE_LED, 1);
    setPinValue(LOWSIDE, 1);
    setPinValue(HIGHSIDE, 0);
    setPinValue(INTEGRATE, 0);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    disablePins();
    waitMicrosecond(1e6);

    //Highside = 1, Lowside = 1, Integrate=0, output = (.15-.25V)
    setPinValue(BLUE_LED, 1);
    setPinValue(LOWSIDE, 1);
    setPinValue(HIGHSIDE, 1);
    setPinValue(INTEGRATE, 0);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    disablePins();
    waitMicrosecond(1e6);

    //Highside = 1, Lowside = 0, Integrate=1, output = (3-3.15V)
    setPinValue(BLUE_LED, 1);
    setPinValue(LOWSIDE, 0);
    setPinValue(HIGHSIDE, 1);
    setPinValue(INTEGRATE, 1);
    waitMicrosecond(1e6);
    setPinValue(BLUE_LED, 0);
    setPinValue(RED_LED, 1);
    disablePins();
    waitMicrosecond(1e6);

    //dump the capacitor
    setPinValue(INTEGRATE, 1);
    setPinValue(LOWSIDE, 1); //discharge //ground both sides of capacitor
    waitMicrosecond(10e3); //wait a reasonable time

}

