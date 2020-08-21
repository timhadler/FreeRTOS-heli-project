/*
 * main.c
 *
 * Main source file for ENCE464 Heli project
 *
 *  Created on: 27/07/2020
 *  Contributers: Hassan Alhujhoj, Abdullah Naeem and Tim Hadler
 *  Last modified: 8.8.2020
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

#include "driverlib/pwm.h"

#include "utils/ustdlib.h"

#include "stdlib.h"

#include "OrbitOLED/OrbitOLEDInterface.h"

#include "OLEDDisplay.h"
#include "constants.h"
#include "motors.h"
#include "yaw.h"
#include "altitude.h"
#include "controllers.h"
#include "userInput.h"
#include "buttons4.h"
#include "debugger.h"
#include "OLEDDisplay.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"




//Create the tasks that will be run by FreeRTOS
void createTasks(void)
{
    xTaskCreate(pollButtons, "Button Poll", 64, (void *) NULL, 3, NULL);
    xTaskCreate(displayOLED, "display", 256, (void *) NULL, 3, NULL);
    xTaskCreate(controller, "controller", 64, (void *) NULL, 2, NULL);
    xTaskCreate(processAlt, "Altitude Calc", 64, (void *) NULL, 4, NULL);
    xTaskCreate(sendData, "UART", 256, (void *) NULL, 3, NULL);
    xTaskCreate(FSM, "Finite State Machine", 64, (void *) NULL, 4, NULL);
}


// Initialize the program
void initialize(void)
{
    // Set clock to 80MHz
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    initButtons();
    initModeSwitch();
    initADC();
    initDisplay();
    initMotors();
    initYaw();
    createTasks();
    initialiseUSB_UART();

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

    // For Reference signal
    SysCtlPeripheralEnable(REF_PERIPH);
    while (!SysCtlPeripheralReady(REF_PERIPH));
    GPIOPinTypeGPIOInput(REF_GPIO_BASE, REF_PIN);
    GPIOPadConfigSet (REF_GPIO_BASE, REF_PIN, GPIO_STRENGTH_4MA,
       GPIO_PIN_TYPE_STD_WPD);

    IntMasterEnable();
}

void main(void)
{
    initialize();
    vTaskStartScheduler();
}
