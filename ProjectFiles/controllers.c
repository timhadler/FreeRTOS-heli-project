/*
 * controllers.c
 *
 *  Created on: 7/08/2020
 *      Author: tch118
 */

#include <stdint.h>
#include "controllers.h"
#include "myMotors.h"


int32_t getAltErr(int32_t cMean) {
    return 0;
}


int32_t getYawErr(int16_t cYaw) {
    return 0;
}


void updateControl(int32_t altError, int32_t yawError) {
    uint8_t uMainDuty = 0;
    uint8_t uTailDuty = 0;


    //TODO calculate control output


    setMotor(MOTOR_M, uMainDuty);
    setMotor(MOTOR_T, uTailDuty);
}


void findRefeference(void) {
    setMotor(MOTOR_T, 0);

    while(GPIOPinRead(REF_GPIO_BASE, REF_PIN)) {
        continue;
    }
}
