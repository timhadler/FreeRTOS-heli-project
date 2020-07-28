/*
 * Main source file for ENCE464 Heli project
 *
 *  Created on: 27/07/2020
 *      Authors: tch118, ...
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"

#include "driverlib/adc.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "utils/ustdlib.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "FreeRTOS.h"
#include "task.h"
#include "buttons4.h"

#include "OLEDDisplay.h"
#include "constants.h"


//******************************************************************
// Global Variables
//******************************************************************
char text_buffer[16];    // global for now


// Initialize the program
void initialize(void) {
    // Set the clock rate to 80 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    initButtons();
    initDisplay();
}


void main(void) {
    initialize();

    //*******************
    // Code to test buttons
    //*******************
    uint8_t count = 0;
    uint32_t clock_rate = SysCtlClockGet();

    while(1) {
        updateButtons();

        if (checkButton (UP) == PUSHED) {
            count++;
            sprintf(text_buffer, "Button Presses %d", count);
            writeDisplay(text_buffer, LINE_1);
        }


        SysCtlDelay(clock_rate / 150);
    }
    //*********************
}
