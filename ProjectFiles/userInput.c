/*
 * user_input.c
 *
 *  Created on: 18/08/2020
 *      Author: tch118
 */

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "buttons4.h"
#include "controllers.h"
#include "userInput.h"


void SwitchModeIntHandler(void) {
    int32_t mSwitch = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Wait for switch to settle, prolly bad practice to put delay in ISR
    SysCtlDelay (SysCtlClockGet () / 150);

    mSwitch = GPIOPinRead(SWITCH_MODE_GPIO_BASE, SWITCH_MODE_PIN);
    if (getState() == LANDED && mSwitch) {
        xSemaphoreGiveFromISR(xTakeOffSemaphore, &xHigherPriorityTaskWoken);
    } else if (getState() == IN_FLIGHT && !mSwitch) {
        xSemaphoreGiveFromISR(xLandSemaphore, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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


void pollButton(void* pvParameters) {
    const uint16_t delay_ms = 1000/BUTTON_POLL_RATE_HZ;
    xButtPollSemaphore = xSemaphoreCreateBinary();

    while (1) {
        if (getState() != IN_FLIGHT) {
            xSemaphoreTake(xButtPollSemaphore, portMAX_DELAY);
        }

        updateButtons();
        if (checkButton (UP) == PUSHED) {
            incAlt();

        } else if (checkButton (DOWN) == PUSHED) {
            decAlt();

        } else if (checkButton (LEFT) == PUSHED) {
            decYaw();

        } else if (checkButton (RIGHT) == PUSHED) {
            incYaw();
        }
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}
