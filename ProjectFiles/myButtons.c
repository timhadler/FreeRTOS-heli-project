/*
 * myButtons.c
 *
 *  Created on: 11/08/2020
 *      Author: tch118
 */

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "buttons4.h"
#include "myFreeRTOS.h"
#include "myButtons.h"
#include "controllers.h"


void SwitchModeIntHandler(void) {
    if (GPIOPinRead(SWITCH_MODE_GPIO_BASE, SWITCH_MODE_PIN)) {
        xSemaphoreGive(xTakeOffSemaphore);
    }

    GPIOIntClear(SWITCH_MODE_GPIO_BASE, SWITCH_MODE_PIN);
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
