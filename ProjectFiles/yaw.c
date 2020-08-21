/*
 *  yaw.c
 *
 *  Contributers: Hassan Alhujhoj, Abdullah Naeem and Tim Hadler
 *  Created on: 6/08/2020
 */

#include "yaw.h"

#define QUEUE_ITEM_SIZE     sizeof(int32_t) //4 bytes which is the size of each ACD sample

void initYaw(void)
{
    // Initialize yaw signals
    SysCtlPeripheralEnable(YAW_PERIPH_GPIO);
    while (!SysCtlPeripheralReady(YAW_PERIPH_GPIO));
    GPIOPinTypeGPIOInput (YAW_GPIO_BASE, CH_A | CH_B);
    GPIODirModeSet(YAW_GPIO_BASE, CH_A | CH_B, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(YAW_GPIO_BASE, CH_A | CH_B, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

    // Set up yaw interrupts - both edge interrupts
    GPIOIntRegister(YAW_GPIO_BASE, YawIntHandler);
    GPIOIntTypeSet(YAW_GPIO_BASE, YAW_CHA_INT_PIN | YAW_CHB_INT_PIN, GPIO_BOTH_EDGES);
    GPIOIntEnable(YAW_GPIO_BASE, YAW_CHA_INT_PIN | YAW_CHB_INT_PIN);

    // set up Reference signal pin
    SysCtlPeripheralEnable(REF_PERIPH);
    while (!SysCtlPeripheralReady(REF_PERIPH));
    GPIOPinTypeGPIOInput(REF_GPIO_BASE, REF_PIN);
    GPIOPadConfigSet (REF_GPIO_BASE, REF_PIN, GPIO_STRENGTH_4MA,
       GPIO_PIN_TYPE_STD_WPD);
}

int32_t getYaw(void)
{
    return (int32_t) (360 * slots) / DISK_INTERRUPTS;
}

void setYawReference(void)
{
    slots = 0;
}

/**
 * This is the interrupt ISR for the yaw sensors
 * 'Gives' a semaphore to defer the event processing to a FreeRTOS task
 */
void YawIntHandler(void)
{
    static int32_t currentState;
    int32_t nextState;

    nextState = GPIOPinRead(YAW_GPIO_BASE, CH_A | CH_B);
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

            // Limits Yaw to 0 - 360 degrees
            if (slots < 0)
            {
                slots = DISK_INTERRUPTS + slots;
            } else if (slots > DISK_INTERRUPTS)
            {
                slots = 0;
            }

        GPIOIntClear(YAW_GPIO_BASE, CH_A | CH_B);
}

