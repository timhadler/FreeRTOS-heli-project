/*
 * controllers.c
 *
 *  Created on: 7/08/2020
 *      Author: tch118
 */

#include <stdint.h>
#include "controllers.h"
#include "myMotors.h"
#include "altitude.h"
#include "myYaw.h"

#define KP_M 10
#define KI_M 0.18
#define KP_T 10
#define KI_T 0.18
#define T_DELTA 0.01
//#define SETALT 0
#define OUTPUT_MAX 95
#define OUTPUT_MIN 5
//#define SETYAW 0


int16_t getAltErr(int16_t tAlt) {
    return tAlt - getAlt();
}


int16_t getYawErr(int16_t tYaw) {
    return tYaw - getYaw();
}


void findReference(void) {
    setMotor(MOTOR_M, 10);
    setMotor(MOTOR_T, 50);

    while(GPIOPinRead(REF_GPIO_BASE, REF_PIN)) {
        continue;
    }
    setMotor(MOTOR_M, 0);
    setMotor(MOTOR_T, 0);
}


void piMainUpdate(int16_t setAlt) {
    //uint8_t uMainDuty = 0;
    //uint8_t uTailDuty = 0;

    static double I;
    double P;
    double control;
    int16_t error_alt;
    double dI;


    error_alt = getAltErr(setAlt); // Error between the set altitude and the actual altitude


    P = KP_M*error_alt;

    dI = KI_M*error_alt*T_DELTA;

    control = P + (I + dI);

    // Enforces output limits
    if (control > OUTPUT_MAX) {
        control = OUTPUT_MAX;
    } else if (control < OUTPUT_MIN) {
        control = OUTPUT_MIN;
    } else {
        I += dI; // Accumulates the a history of the error in the integral
    }
    setMotor(MOTOR_M, control);
}


void piTailUpdate(int16_t setYaw) {

    static double I;
    double P;
    double control;
    int16_t error_yaw;
    double dI;

    error_yaw = getYawErr(setYaw); // Error between the set altitude and the actual altitude


    P = KP_M*error_yaw;

    dI = KI_M*error_yaw*T_DELTA;

    control = P + (I + dI);

    // Enforces output limits
    if (control > OUTPUT_MAX) {
        control = OUTPUT_MAX;
    } else if (control < OUTPUT_MIN) {
        control = OUTPUT_MIN;
    } else {
        I += dI; // Accumulates the a history of the error in the integral
    }
    setMotor(MOTOR_T, 37);
}
