//Jorge Avila
//test resistance
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
    initHw();
    initMeasure();
    initUart0();
    setUart0BaudRate(115200, 40e6);
    putsUart0("Welcome to testing the board!\n\r");
    USER_DATA data;

    bool valid = false;
    while (true)
    {
        putsUart0("\nJorge's-CLI$>> ");
        getsUart0(&data);
        putsUart0("\n");

        if (isCommand(&data, "RESOFF", 0))
        {
            valid = true;
        }
        if(isCommand(&data, "measure", 1)){
            char * type = getFieldString(&data, 1);
            if(stringCompare(type,"resistance")){

                valid = true;
            }
            valid = true;
        }
        if (!valid)
        {
            putsUart0("unknown command..\n");
        }
    }

    //outside the while loop
}

