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



void blinkLED(void *pvParameters) {\
    uint8_t pin = (*(uint8_t *) pvParameters);
    uint8_t current = 0;

    while(1) {
        current ^= pin;
        GPIOPinWrite(LED_GPIO_BASE, pin, current);
        vTaskDelay(LED_BLINK_RATE / portTICK_RATE_MS);
    }
}


// Initialize the program
void initialize(void) {
    // Set the clock rate to 80 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    initButtons();
    initDisplay();
    static uint8_t led = LED_RED_PIN;

    // For LED blinky task - initialize GPIO port F and then pin #1 (red) for output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);                // activate internal bus clocking for GPIO port F
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));        // busy-wait until GPIOF's bus clock is ready

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);         // PF_1 as output
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);    // doesn't need too much drive strength as the RGB LEDs on the TM4C123 launchpad are switched via N-type transistors
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);               // off by default

    if (pdTRUE != xTaskCreate(blinkLED, "Happy LED go blink blink", 32, (void *) &led, 4, NULL))
    {
        while(1);               // Oh no! Must not have had enough memory to create the task.
    }
}


void main(void) {
    initialize();
    vTaskStartScheduler();

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
