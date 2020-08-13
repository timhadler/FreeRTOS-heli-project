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

//#include "inc/tm4c123gh6pm.h"
#include "driverlib/pwm.h"

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
#include "myButtons.h"


//******************************************************************
// Global Variables
//******************************************************************

static int16_t targetAlt;
static int16_t targetYaw;


//******************************************************************
// Functions
//******************************************************************
void displayOLED(void* pvParameters) {
    char text_buffer[16];
    while(1) {
        // Display Height
        sprintf(text_buffer, "Altitude: %d%%", getAlt());
        writeDisplay(text_buffer, LINE_1);

        // Display yaw
        sprintf(text_buffer, "Yaw: %d deg", getYaw());
        writeDisplay(text_buffer, LINE_2);

        sprintf(text_buffer, "Target Alt: %d%%", targetAlt);
        writeDisplay(text_buffer, LINE_3);

        sprintf(text_buffer, "Target Yaw: %ddeg",targetYaw);
        writeDisplay(text_buffer, LINE_4);

        taskDelayMS(1000/DISPLAY_RATE_HZ);
    }
}


void controller(void* pvParameters) {
    findReference();

    while(1) {
        targetAlt = getTargetAlt();
        targetYaw = getTargetYaw();

        piMainUpdate(targetAlt);
        piTailUpdate(targetYaw);

        taskDelayMS(1000/CONTROLLER_RATE_HZ);
    }
}


void createTasks(void) {
    createTask(pollButton, "Button Poll", 256, (void *) NULL, 3, NULL);
    createTask(displayOLED, "display", 256, (void *) NULL, 3, NULL);
    createTask(controller, "controller", 56, (void *) NULL, 2, NULL);
    createTask(processAlt, "Altitude Calc", 128, (void *) NULL, 4, NULL);
}


// Initialize the program
void initialize(void) {
    // Set clock to 80MHz
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    initButtons();
    initADC();
    initDisplay();
    initBuffer();
    initMotors();
    initYaw();
    createTasks();

    //// BUTTONS...
    GPIOPinTypeGPIOInput (LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
    GPIOPadConfigSet (LEFT_BUT_PORT_BASE, LEFT_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPU);


    //---Unlock PF0 for the right button:
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= GPIO_PIN_0; //PF0 unlocked
    GPIO_PORTF_LOCK_R = GPIO_LOCK_M;
    GPIOPinTypeGPIOInput (RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
    GPIOPadConfigSet (RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPU);
    ////


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);                // For Reference signal
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));

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

    // Should never get here if startFreeRTOS is not un-commented

    char text_buffer[16];

    setMotor(MOTOR_M, 44);
    setMotor(MOTOR_T, 37);

    while(1) {
        uint16_t avg = 5;
        sprintf(text_buffer, "ADC AVG: %d", avg);
        writeDisplay(text_buffer, LINE_2);
    }
}
