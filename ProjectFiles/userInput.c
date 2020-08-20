/*
 * user_input.c
 *
 *  Created on: 18/08/2020
 *      Author: tch118
 */

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


void initModeSwitch(void)
{
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


uint8_t checkButtSequence(uint8_t butt)
{
    uint8_t completedSeq = 0;
    static TickType_t lastTime;
    TickType_t currTime = xTaskGetTickCount();

    if ((currTime - lastTime) > pdMS_TO_TICKS(TIME_THRESHOLD_MS))
    {
        seq1_pos = 0;
        seq2_pos = 0;
    }

    if (butt == sequence_1[seq1_pos])
    {
        seq1_pos++;
    } else {
        seq1_pos = 0;
    }

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

        if (seq == SEQUENCE_1)
        {
            setMode1();
        } else if (seq == SEQUENCE_2)
        {
            setMode2();
        }
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}
