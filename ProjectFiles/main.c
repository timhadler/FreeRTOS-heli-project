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
//#include "driverlib/interrupt.h"

//#include "driverlib/pwm.h"

#include "utils/ustdlib.h"

#include "stdlib.h"

//#include "OrbitOLED/OrbitOLEDInterface.h"

#include "OLEDDisplay.h"
#include "constants.h"
//#include "motors.h"
#include "yaw.h"
#include "altitude.h"
#include "controllers.h"
#include "userInput.h"
//#include "buttons4.h"
#include "debugger.h"
#include "OLEDDisplay.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"




// Initialize the program
void initialize(void)
{
    // Set clock to 80MHz
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // INitialise all the perphierals
    initUserInput();
    initModeSwitch();
    initADC();
    initDisplay();
    initMotors();
    initYaw();
    initialiseUSB_UART();

    // Create freeRTOS tasks
    xTaskCreate(pollButtons, "Button Polling", 64, (void *) NULL, 3, NULL);
    xTaskCreate(displayOLED, "OLED Display", 256, (void *) NULL, 3, NULL);
    xTaskCreate(controller, "Controller", 64, (void *) NULL, 2, NULL);
    xTaskCreate(processAlt, "Altitude Calc", 64, (void *) NULL, 4, NULL);
    xTaskCreate(sendData, "UART", 256, (void *) NULL, 3, NULL);
    xTaskCreate(FSM, "Finite State Machine", 64, (void *) NULL, 4, NULL);

    IntMasterEnable();
}

void main(void)
{
    initialize();
    vTaskStartScheduler();
}
