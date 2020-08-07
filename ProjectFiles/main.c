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

#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "utils/ustdlib.h"
#include "stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"

#include "OLEDDisplay.h"
#include "constants.h"
#include "buffer.h"
#include "myTasks.h"
#include "myFreeRTOS.h"
#include "myMotors.h"
#include "myYaw.h"
#include "altitude.h"
#include "controllers.h"


//******************************************************************
// Global Variables
//******************************************************************

int16_t yaw;
int32_t mean;

uint8_t altitude;


//******************************************************************
// Functions
//******************************************************************
void displayOLED(void* pvParameters) {
    char text_buffer[16];

    while(1) {
        // Display Height
        sprintf(text_buffer, "Altitude: %d %%", altitude);
        writeDisplay(text_buffer, LINE_1);

        // Display yaw
        sprintf(text_buffer, "Yaw: %d", yaw);
        writeDisplay(text_buffer, LINE_2);

        // Display motor PWMs
        taskDelayMS(1000/DISPLAY_RATE_HZ);
    }
}


// Dunno what to call this yet
// Will use getYaw and getHeight functions in here
// Initiate PI controllers with semaphores
void controller(void* pvParameters) {
    int32_t yawErr = 0;
    int32_t altErr = 0;

    while(1) {
        yaw = getYaw();
        altitude = getAlt();

        //yawErr = getYawErr(yaw);
        //altErr = getAltErr(mean);

        updateControl(altErr, yawErr);

        //mean = getMeanVal();
        taskDelayMS(1000/CONTROLLER_RATE_HZ);
    }
}


void createTasks(void) {
    static uint8_t led = LED_RED_PIN;

    createTask(blinkLED, "Happy LED go blink blink", 32, (void *) &led, 1, NULL);
    createTask(pollButton, "Button Poll", 200, (void *) NULL, 3, NULL);
    createTask(processYaw, "Yaw stuff", 200, (void *) NULL, 4, NULL);
    createTask(displayOLED, "display", 200, (void *) NULL, 3, NULL);

    createTask(controller, "controller", 50, (void *) NULL, 2, NULL);
    createTask(processAlt, "Altitude Calc", 200, (void *) NULL, 3, NULL);
}


// Initialize the program
void initialize(void) {
    // Set clock to 80MHz
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    initADC();
    initButtons();
    initDisplay();
    initBuffer();
    initMotors();
    initYaw();
    createTasks();

    // For LED blinky task - initialize GPIO port F and then pin #1 (red) for output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);                // activate internal bus clocking for GPIO port F
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));        // busy-wait until GPIOF's bus clock is ready

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);         // PF_1 as output
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);    // doesn't need too much drive strength as the RGB LEDs on the TM4C123 launchpad are switched via N-type transistors
    GPIOPinWrite(LED_GPIO_BASE, LED_RED_PIN, 0x00);               // off by default
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);         // PF_1 as output
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);    // doesn't need too much drive strength as the RGB LEDs on the TM4C123 launchpad are switched via N-type transistors
    GPIOPinWrite(LED_GPIO_BASE, LED_GREEN_PIN, 0x00);

    IntMasterEnable();
}


void main(void) {
    initialize();
    createSemaphores();
    startFreeRTOS();

    char text_buffer[16];

    // Should never get here if startFreeRTOS is not un-commented
    setMotor(MOTOR_M, 44);
    setMotor(MOTOR_T, 37);
    while(1) {
        uint16_t avg = 5;
        sprintf(text_buffer, "ADC AVG: %d", avg);
        writeDisplay(text_buffer, LINE_2);
    }
}
