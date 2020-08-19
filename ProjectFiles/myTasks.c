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


