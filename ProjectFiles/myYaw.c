/*
 * myYaw.c
 *
 *  Created on: 6/08/2020
 *      Author: tch118
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "myFreeRTOS.h"
#include "semphr.h"
#include "myYaw.h"
#include "OLEDDisplay.h"

enum quadrature {A=0, B=1, C=3, D=2}; // Sets the values for the finite state machine

int16_t yaw;


void initYaw(void) {
    // Initialize yaw signals
    SysCtlPeripheralEnable(YAW_PERIPH_GPIO);
    GPIODirModeSet(YAW_GPIO_BASE, CH_A | CH_B, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(YAW_GPIO_BASE, CH_A | CH_B, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

    // Set up yaw interrupts - both edge interrupts
    GPIOIntTypeSet(YAW_GPIO_BASE, YAW_CHA_INT_PIN | YAW_CHB_INT_PIN, GPIO_BOTH_EDGES);
    GPIOIntRegister(YAW_GPIO_BASE, YawIntHandler);
    GPIOIntEnable(YAW_GPIO_BASE, YAW_CHA_INT_PIN | YAW_CHB_INT_PIN);
}


// This is the interrupt ISR for the yaw sensors
// 'Gives' a semaphore to defer the event processing to a FreeRTOS task
void YawIntHandler(void) {
    uint32_t intStatus = 0;
    BaseType_t xHigherPriorityTaskWoken;

    xHigherPriorityTaskWoken = pdFALSE;
    intStatus = GPIOIntStatus(YAW_GPIO_BASE, true);

    if (pdPASS != xSemaphoreGiveFromISR(xYawSemaphore, &xHigherPriorityTaskWoken)) {
        //while(1) {};
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    GPIOIntClear(YAW_GPIO_BASE, intStatus);
}


int16_t getYaw(void) {
    return yaw;
}


void processYaw(void* pvParameters) {
    int32_t slots = 0;
    int32_t currentState = 0;
    int32_t nextState = 0;

    while(1) {
        if (pdPASS != xSemaphoreTake(xYawSemaphore, portMAX_DELAY)) {
           while(1) {};
        }

        nextState = GPIOPinRead(YAW_GPIO_BASE, CH_A | CH_B);
        /* A finite state machine has been used looks at the current state and the next state.
           There are four states read from the pins: A, B, C, and D, where A = 00, B = 01, C = 11, and D = 10. */
        switch(currentState)
        {
            case A:
                switch(nextState)
                {
                case B:
                    slots--; // Decreases the slot count (anticlockwise)
                    break;
                case D:
                    slots++; // Increases the slot count (clockwise)
                    break;
                }
                break;
            case B:
                switch(nextState)
                {
                case A:
                    slots++; // Increases the slot count (clockwise)
                    break;
                case C:
                    slots--; // Decreases the slot count (anticlockwise)
                    break;
                }
                break;
            case C:
                switch(nextState)
                {
                case B:
                    slots++; // Increases the slot count (clockwise)
                    break;
                case D:
                    slots--; // Decreases the slot count (anticlockwise)
                    break;
                }
                break;
            case D:
                switch(nextState)
                {
                case A:
                    slots--; // Decreases the slot count (anticlockwise)
                    break;
                case C:
                    slots++; // Increases the slot count (clockwise)
                    break;
                }
                break;
        }
        currentState = nextState;

        // Limits the yaw to +-180 degees from the reference point
        if (slots == 224 || slots == -224) {
            slots = slots*-1; // Switches the sign of yaw angle
        }
        yaw = (360 * slots) / DISK_INTERRUPTS;
    }
}
