/*
 * controllers.c
 *
 *  Created on: 7/08/2020
 *      Author: tch118
 */

#include "controllers.h"
#include "myMotors.h"
#include "altitude.h"
#include "myYaw.h"

#define KP_M 1
#define KI_M 0.18
#define KP_T 1
#define KI_T 0.18
#define T_DELTA 0.01
#define SETALT 20
#define OUTPUT_MAX 95
#define OUTPUT_MIN 5
#define SETYAW 2


double getAltErr(void) {
    return SETALT - getAlt();
}


double getYawErr(void) {
    return SETYAW - getYaw();
}


void piMainUpdate(void) {
    //uint8_t uMainDuty = 0;
    //uint8_t uTailDuty = 0;

    static double I;
    double P;
    double control;
    double error_alt;
    double dI;


    error_alt = getAltErr(); // Error between the set altitude and the actual altitude


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


void piTailUpdate(void) {

    static double I;
    double P;
    double control;
    double error_yaw;
    double dI;

    error_yaw = getYawErr(); // Error between the set altitude and the actual altitude


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
    setMotor(MOTOR_T, control);

}
