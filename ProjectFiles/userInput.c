/*
 * user_input.c
 *
 *  Created on: 18/08/2020
 *      Author: tch118
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

//#include "buttons4.h"
#include "controllers.h"
#include "userInput.h"

void SwitchModeIntHandler(void)
{
    int32_t mSwitch = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Wait for switch to settle, prolly bad practice to put delay in ISR
    SysCtlDelay (SysCtlClockGet () / 150);

    mSwitch = GPIOPinRead(SWITCH_MODE_GPIO_BASE, SWITCH_MODE_PIN);

    if (getState() == LANDED && mSwitch)
    {
        xSemaphoreGiveFromISR(xTakeOffSemaphore, &xHigherPriorityTaskWoken);
    } else if (getState() == IN_FLIGHT && !mSwitch)
    {
        xSemaphoreGiveFromISR(xLandSemaphore, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    GPIOIntClear(SWITCH_MODE_GPIO_BASE, SWITCH_MODE_PIN);
}


void initUserInput(void)
{
    // init buttons from buttons4.c
    initButtons();

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
}


/* Checks to see if a button press is part of a sequence for the two special modes
 * returns the sequence once completed, NULL if no sequence is completed
*/
uint8_t checkButtSequence(uint8_t butt)
{
    uint8_t completedSeq = 0;
    static TickType_t lastTime;
    TickType_t currTime = xTaskGetTickCount();

    // Restes the sequence counter after a threshold period
    if ((currTime - lastTime) > pdMS_TO_TICKS(TIME_THRESHOLD_MS))
    {
        seq1_pos = 0;
        seq2_pos = 0;
    }

    // Advances sequence counter if butt matches the next button in the sequence, resets counter if not
    if (butt == sequence_1[seq1_pos])
    {
        seq1_pos++;
    } else {
        seq1_pos = 0;
    }

    // Advances sequence counter if butt matches the next button in the sequence, resets counter if not
    if (butt == sequence_2[seq2_pos])
    {
        seq2_pos++;
    } else {
        seq2_pos = 0;
    }

    if (seq1_pos == SEQUENCE_LENGTH)
    {
        completedSeq = SEQUENCE_1;
        seq1_pos = 0;
        seq2_pos = 0;

    } else if (seq2_pos == SEQUENCE_LENGTH)
    {
        completedSeq = SEQUENCE_2;
        seq1_pos = 0;
        seq2_pos = 0;

    } else
    {
        completedSeq = NULL;
    }

    lastTime = currTime;
    return completedSeq;
}


// Tassk for polling buttons, blocks itself if not in the IN_FLIGHT mode
void pollButtons(void* pvParameters)
{
    const uint16_t delay_ms = 1000/BUTTON_POLL_RATE_HZ;
    xButtPollSemaphore = xSemaphoreCreateBinary();
    uint8_t seq = 0;

    while (1)
    {
        if (getState() != IN_FLIGHT)
        {
            xSemaphoreTake(xButtPollSemaphore, portMAX_DELAY);
        }

        updateButtons();
        if (checkButton (UP) == PUSHED)
        {
            incAlt();
            seq = checkButtSequence(UP);

        } else if (checkButton (DOWN) == PUSHED)
        {
            decAlt();
            seq = checkButtSequence(DOWN);

        } else if (checkButton (LEFT) == PUSHED)
        {
            decYaw();
            seq = checkButtSequence(LEFT);

        } else if (checkButton (RIGHT) == PUSHED)
        {
            incYaw();
            seq = checkButtSequence(RIGHT);
        }

        // If a sequence has been met, trigger the corresponding special mode
        if (seq == SEQUENCE_1)
        {
            setMode1();
        } else if (seq == SEQUENCE_2)
        {
            setMode2();
            seq = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}
