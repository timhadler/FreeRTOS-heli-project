/*
 * myTasks.c
 *
 *  Created on: 1/08/2020
 *      Author: Tim Hadler
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "constants.h"
#include "buttons4.h"
#include "myFreeRTOS.h"
#include "myTasks.h"
#include "OLEDDisplay.h"


void blinkLED(void* pvParameters) {
    uint8_t pin = (*(uint8_t *) pvParameters);
    uint8_t current = 0;

    while(1) {

        current ^= pin;
        GPIOPinWrite(LED_GPIO_BASE, pin, current);
        taskDelayMS(LED_BLINK_PERIOD_MS / 2);
    }
}


void pollButton(void* pvParameters) {
    //static uint8_t count = 0;
    uint8_t current = 0;

    // This code atm is just testing to make sure the task works properly, which it does
    while (1) {
        updateButtons();
        current ^= LED_GREEN_PIN;
        if (checkButton (UP) == PUSHED) {
            //count++;
            current ^= LED_GREEN_PIN;
            GPIOPinWrite(LED_GPIO_BASE, LED_GREEN_PIN, current);
        }
        taskDelayMS(1000/BUTTON_POLL_RATE_HZ);
    }
}
