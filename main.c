//Jorge Avila
//mavID: 1001543128
//Spring 2021 Embedded 2 basic multimeter project

//header files
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "measurments.h"
#include "clock.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"

//increase the stack to 4096 for sprintf()
void initHw()
{
    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();

}

int main(void)

{
    //initializations of hardware,Peripheral, etc
    initHw();
    initUart0();
    setUart0BaudRate(115200, 40e6);
    initMeasure();

    USER_DATA data;
    //putsUart0("Welcome to testing the board!\n\r");
    putsUart0("Welcome to Embedded 2 Project!\n");
    putsUart0("What would your dusty ass like to measure today?\n");

    bool valid = false;

    //where all of our options be
    while (true)
    {
        putsUart0("\n");
        putsUart0("\nJorge's-CLI$>> ");
        getsUart0(&data);
        putsUart0("\n");
        parseFields(&data);

        //measuring block of code
        if (isCommand(&data, "resistor", 0))
        {
            //char *type = getFieldString(&data, 1);

            putsUart0("Measuring Resistance\n");
            uint32_t res = measureResistance();
            putsUart0("The value of the resistor is: ");
            //ATOI(res);
            char res_str[300];
            sprintf(res_str, "%d", res);
            putsUart0(res_str);

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
            for (i = 0; i < 15; i++)
            {
                putcUart0('\n');
            }
        }
        if (!valid)
        {
            putsUart0("unknown command..\n");
        }
    }

    //outside the while loop
}

