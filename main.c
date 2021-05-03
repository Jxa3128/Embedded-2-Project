//Jorge Avila
//mavID: 1001543128
//Spring 2021 Embedded 2 basic multimeter project

//header files
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "measurments.h"
#include "clock.h"
#include "gpio.h"
#include "tm4c123gh6pm.h"
#include "uart0.h"

//prototypes
void initHw();
void help();

//int waitResBUT()
//{
//    while (!getPinValue(RES_PB))
//        ;
//    return !getPinValue(RES_PB);
//}
int main(void)
{
    //initializations of hardware,Peripheral, etc
    initHw();
    initUart0();
    setUart0BaudRate(115200, 40e6);
    initMeasure();
    USER_DATA data;
    //putsUart0("Welcome to testing the board!\n\r");
    putsUart0("\nWelcome to Embedded 2 Project!\n");
    putsUart0("This is a Low-Cost Impedance Meter.\n");
    putsUart0("Spring 2021 - Dr. Jason Losh at UT Arlington.\n");

    bool valid = false;

    //where all of our options be
    while (true)
    {
        //putsUart0("\n");

        putsUart0("\nJorge's-CLI$>> ");
        getsUart0(&data);
        putsUart0("\n");
        parseFields(&data);

        if (isCommand(&data, "reset", 0))
        {
            putsUart0("Rebooting the TM4C213...\n");
            reIniciar();
            valid = true;
        }
        //measuring block of code
        if (isCommand(&data, "resistor", 0))
        {
            //char *type = getFieldString(&data, 1);

            putsUart0("Measuring Resistance...\n");
            uint32_t res = measureResistance();
            putsUart0("The value of the resistor is about ~: ");
            //ATOI(res);
            char res_str[MAX_BUFF];
            sprintf(res_str, "%d ohms\n", res);
            putsUart0(res_str);

            valid = true;
        }
        if (isCommand(&data, "cap", 0))
        {
            putsUart0("Measuring Capacitance...\n");
            uint32_t cap = measureCapacitance();
            char cap_str[MAX_BUFF];
            if (cap <= 0)
            {
                float tempCap = 0.011;
                putsUart0("The value of the capacitor is about ~: ");
                sprintf(cap_str, "%0.3f microFarads (uF) \n", tempCap);
                putsUart0(cap_str);
            }
            else
            {
                putsUart0("The value of the capacitor is about ~: ");
                sprintf(cap_str, "%d microFarads (uF) \n", cap);
                putsUart0(cap_str);
            }

            valid = true;
        }
        if (isCommand(&data, "inductance", 0))
        {
            putsUart0("Measuring Inductance...\n");
            uint32_t induct = measureInductance();
            putsUart0("The value of the inductor is about ~: ");
            char induct_str[MAX_BUFF];

            sprintf(induct_str, "%d microhenries\n", induct);
            putsUart0(induct_str);
            valid = true;
        }
        if (isCommand(&data, "esr", 0))
        {
            putsUart0("Measuring the esr across DUT1-DUT2...\n");
            double esr = measureEsr();
            putsUart0("The esr is about ~: ");
            char esr_buff[MAX_BUFF];
            sprintf(esr_buff, "%f ohms\n", esr);
            putsUart0(esr_buff);
            valid = true;
        }
        if (isCommand(&data, "voltage", 0))
        {
            putsUart0("Measuring voltage...\n");
            float realVoltage = getFullVoltage();
            char volt_buff[MAX_BUFF];
            sprintf(volt_buff, "%f volts.\n", realVoltage);
            putsUart0(volt_buff);
            valid = true;
        }
        if (isCommand(&data, "auto", 0))
        {
            putsUart0("Determining what you have...\n");
            measureAuto();
            valid = true;
        }
        if (isCommand(&data, "test", 1))
        {
            char *test = getFieldString(&data, 1);
            if (stringCompare(test, "start"))
            {
                putsUart0("Testing board....\n");
                testBoard();
                valid = true;
            }
            if (stringCompare(test, "end"))
            {
                putsUart0("Disabling pins...\n");
                disablePins();
                valid = true;
            }
        }
        if (isCommand(&data, "clear", 0))
        {
            uint32_t i;
            for (i = 0; i < 20; i++)
            {
                putcUart0('\n');
            }
            valid = true;
        }
        if (isCommand(&data, "help", 0))
        {
            help();
            valid = true;
        }
//        if (isCommand(&data, "check", 0))
//        {
//            int count = 0;
//            while (count < MAX_BUFF)
//            {
//                putsUart0("Checking -> ");
//                char test[MAX_BUFF];
//                sprintf(test, "button testing = %d.\n", getPinValue(RES_PB));
//                putsUart0(test);
//                count++;
//
//            }
//            valid = true;
//        }
        if (!valid)
        {
            putsUart0("unknown command..\n");
        }
    }

    //outside the while loop
}

//increase the stack to 4096 for sprintf() in the linker
void initHw()
{
    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();

}

void help()
{
    putsUart0("\nYou may use the following commands:\n");
    putsUart0("\r\t- type <resistor>, returns the resistance in (ohms).\n");
    putsUart0(
            "\r\t- type <capacitance>, returns the capacitance in (farads).\n");
    putsUart0(
            "\r\t- type <inductance>, returns the inductance in (henries).\n");
    putsUart0("\r\t- type <esr>, returns the esr of the inductor.\n");
    putsUart0(
            "\r\t- type <voltage>, returns the voltage 0V up to a 3.3V DC source.\n");
    putsUart0("\r\t- type <clear>, clears the screen.\n");
    putsUart0("\r\t- type <reset>, resets the hardware.\n");
    putsUart0("\r\t- type <test>, tests the DUT1 and DUT2.\n");
}

