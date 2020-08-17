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
#include "driverlib/uart.h"
#include "utils/ustdlib.h"
#include "stdlib.h"

#include "driverlib/pwm.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"

#include "OLEDDisplay.h"
#include "constants.h"
//#include "myFreeRTOS.h"
#include "myMotors.h"
#include "myYaw.h"
#include "altitude.h"
#include "controllers.h"
#include "control_command.h"
#include "debugger.h"


//******************************************************************
// Global Variables
//******************************************************************



//******************************************************************
// Functions
//******************************************************************
void SwitchModeIntHandler(void) {
    if (GPIOPinRead(SWITCH_MODE_GPIO_BASE, SWITCH_MODE_PIN)) {
        xSemaphoreGive(xTakeOffSemaphore);
    }

    GPIOIntClear(SWITCH_MODE_GPIO_BASE, SWITCH_MODE_PIN);
}


void displayOLED(void* pvParameters) {
    const uint16_t delay_ms = 1000/DISPLAY_RATE_HZ;

    char text_buffer[16];
    while(1) {
        // Display Height
        sprintf(text_buffer, "Altitude: %d%%", GPIOPinRead(REF_GPIO_BASE, REF_PIN));
        writeDisplay(text_buffer, LINE_1);

        // Display yaw
        sprintf(text_buffer, "Yaw: %d", getYaw());
        writeDisplay(text_buffer, LINE_2);

        sprintf(text_buffer, "Target Alt: %d%%", targetAlt);
        writeDisplay(text_buffer, LINE_3);

        sprintf(text_buffer, "Target Yaw: %d", targetYaw);
        writeDisplay(text_buffer, LINE_4);


        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}


void createTasks(void) {
    xTaskCreate(pollButton, "Button Poll", 400, (void *) NULL, 3, NULL);
    xTaskCreate(displayOLED, "display", 200, (void *) NULL, 3, NULL);
    xTaskCreate(controller_command, "controller", 300, (void *) NULL, 2, NULL);
    xTaskCreate(processAlt, "Altitude Calc", 128, (void *) NULL, 4, NULL);
    //xTaskCreate(sendData, "UART", 200, (void *) NULL, 5, NULL);
    xTaskCreate(takeOff, "Take off sequence", 56, (void *) NULL, 3, NULL);
    xTaskCreate(debug_log_task, "debug_log_task", 200, (void *) NULL, 4, NULL);

}


void initModeSwitch(void) {
    // initialize mode switch
    SysCtlPeripheralEnable(MODE_PERIPH_GPIO);
    GPIOPinTypeGPIOInput (SWITCH_MODE_GPIO_BASE, SWITCH_MODE_PIN);
    GPIOPadConfigSet (SWITCH_MODE_GPIO_BASE, SWITCH_MODE_PIN, GPIO_STRENGTH_2MA,
          GPIO_PIN_TYPE_STD_WPD);

    // Set up mode switch interrupts
    // Interrupt on both edges
    GPIOIntTypeSet(SWITCH_MODE_GPIO_BASE, SWITCH_MODE_INT_PIN, GPIO_BOTH_EDGES);
    GPIOIntRegister(SWITCH_MODE_GPIO_BASE, SwitchModeIntHandler);
    GPIOIntEnable(SWITCH_MODE_GPIO_BASE, SWITCH_MODE_INT_PIN);
}


//********************************************************
// initialiseUSB_UART - 8 bits, 1 stop bit, no parity
//********************************************************


// Initialize the program
void initialize(void) {
    // Set clock to 80MHz
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    initButtons();
    initModeSwitch();
    initADC();
    initDisplay();
    initMotors();
    initYaw();
    createTasks();
    initdebugger();

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


    SysCtlPeripheralEnable(REF_PERIPH);                // For Reference signal
    while (!SysCtlPeripheralReady(REF_PERIPH));
    GPIOPinTypeGPIOInput(REF_GPIO_BASE, REF_PIN);
    GPIOPadConfigSet (REF_GPIO_BASE, REF_PIN, GPIO_STRENGTH_4MA,
       GPIO_PIN_TYPE_STD_WPD);

    GPIOPinWrite(LED_GPIO_BASE, LED_RED_PIN, 0x00);               // off by default
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);         // PF_1 as output
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);    // doesn't need too much drive strength as the RGB LEDs on the TM4C123 launchpad are switched via N-type transistors
    GPIOPinWrite(LED_GPIO_BASE, LED_GREEN_PIN, 0x00);

    IntMasterEnable();
}


void main(void) {
    initialize();
    vTaskStartScheduler();
}
