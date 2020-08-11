/*
 * myYaw.c
 *
 *  Created on: 6/08/2020
 *      Author: tch118
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "driverlib/interrupt.h"
#include "myFreeRTOS.h"
#include "myYaw.h"
#include "OLEDDisplay.h"


//******************************************************************
// Global Variables
//******************************************************************
enum quadrature {A=0, B=1, C=3, D=2}; // Sets the values for the finite state machine
int32_t currentState;
static int32_t slots;
//static volatile int32_t yaw;

/* Constants */
#define QUEUE_SIZE 1 // Matches the number of samples per period of jitter, ensuring it will not significantly deviate
#define QUEUE_ITEM_SIZE sizeof(int32_t) //4 bytes which is the size of each ACD sample


/* FreeRTOS variables*/



void initYaw(void) {
    /* Create a a FreeRTOS queue for YAW final values */
    YAW_Queue = xQueueCreate(QUEUE_SIZE, QUEUE_ITEM_SIZE);
    if(YAW_Queue == NULL){
        while(1);
    }

    // Initialize yaw signals
    SysCtlPeripheralEnable(YAW_PERIPH_GPIO);
    while (!SysCtlPeripheralReady(YAW_PERIPH_GPIO));
    GPIODirModeSet(YAW_GPIO_BASE, CH_A | CH_B, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(YAW_GPIO_BASE, CH_A | CH_B, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

    // Set up yaw interrupts - both edge interrupts
    GPIOIntRegister(YAW_GPIO_BASE, YawIntHandler);
    GPIOIntTypeSet(YAW_GPIO_BASE, YAW_CHA_INT_PIN | YAW_CHB_INT_PIN, GPIO_BOTH_EDGES);
    GPIOIntEnable(YAW_GPIO_BASE, YAW_CHA_INT_PIN | YAW_CHB_INT_PIN);
}


int32_t getYaw(void){
    int32_t yaw = 0;
    if(xQueuePeek(YAW_Queue, &yaw, pdMS_TO_TICKS(10))){
        while(1);
    }
    return yaw;
}


// This is the interrupt ISR for the yaw sensors
// 'Gives' a semaphore to defer the event processing to a FreeRTOS task
void YawIntHandler(void) {
    int32_t yaw;
    int32_t nextState;
    nextState = GPIOIntStatus(YAW_GPIO_BASE, CH_A | CH_B);

    switch(currentState)
            {
                case A:
                    switch(nextState)
                    {
                    case B:
                        slots--; // Decreases the slot count (CCW)
                        break;
                    case D:
                        slots++; // Increases the slot count (CW)
                        break;
                    }
                    break;
                case B:
                    switch(nextState)
                    {
                    case A:
                        slots++; // Increases the slot count (CW)
                        break;
                    case C:
                        slots--; // Decreases the slot count (CCW)
                        break;
                    }
                    break;
                case C:
                    switch(nextState)
                    {
                    case B:
                        slots++; // Increases the slot count (CW)
                        break;
                    case D:
                        slots--; // Decreases the slot count (CCW)
                        break;
                    }
                    break;
                case D:
                    switch(nextState)
                    {
                    case A:
                        slots--; // Decreases the slot count (CCW)
                        break;
                    case C:
                        slots++; // Increases the slot count (CW)
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
        if(xQueueOverwriteFromISR(YAW_Queue, &yaw, pdFALSE) != pdPASS){
            while(1);
        }
        GPIOIntClear(YAW_GPIO_BASE, CH_A | CH_B);

}

