/*
 * myButtons.c
 *
 *  Created on: 11/08/2020
 *      Author: tch118
 */

#include <stdint.h>
#include "buttons4.h"
#include "myFreeRTOS.h"
#include "myButtons.h"


static int16_t targetAlt;
static int16_t targetYaw;

int16_t getTargetAlt(void) {
    return targetAlt;
}


int16_t getTargetYaw(void) {
    return targetYaw;
}


void pollButton(void* pvParameters) {
    targetAlt = 0;
    targetYaw = 0;

    while (1) {
        updateButtons();
        if (checkButton (UP) == PUSHED) {
            targetAlt += 10;
        } else if (checkButton (DOWN) == PUSHED) {
            targetAlt -= 10;
        } else if (checkButton (LEFT) == PUSHED) {
            targetYaw -= 15;
        } else if (checkButton (RIGHT) == PUSHED) {
            targetYaw += 15;
        }

        if (targetAlt > 100) {
            targetAlt = 100;
        }
        if (targetAlt < 0) {
            targetAlt = 0;
        }
        if (targetYaw > 180) {
            targetYaw = -165;
        }
        if (targetYaw < -165) {
            targetYaw = 180;
        }
        taskDelayMS(1000/BUTTON_POLL_RATE_HZ);
    }
}
