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


void processYaw(void* pvParameters) {
    static uint8_t c = 0;
    char text_buffer[16];
    while(1) {
        if (pdPASS != xSemaphoreTake(xYawSemaphore, portMAX_DELAY)) {
           while(1) {};
        }

        sprintf(text_buffer, "Count: %d", c);
        writeDisplay(text_buffer, 1);
        c++;
        taskDelayMS(1);




        // Do yaw stuff
    }
}
