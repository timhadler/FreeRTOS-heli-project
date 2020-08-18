/*
 * controllers.c
 *
 *  Created on: 7/08/2020
 *      Author: tch118
 */

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "controllers.h"
#include "myMotors.h"
#include "altitude.h"
#include "myYaw.h"
#include "buttons4.h"
#include "debugger.h"


static int16_t reference;
int counter = 0;



int16_t getReference(void) {
    return reference;
}

int16_t getAltErr(int16_t tAlt) {
    return tAlt - getAlt();
}


int16_t getYawErr(int16_t tYaw) {
    int16_t error = 0;
    int16_t currYaw = getYaw();

    // Calculates error
    if (tYaw > 260 && currYaw < 90) {
        error = tYaw - 360 - currYaw;

    } else if (tYaw < 90 && currYaw > 260) {
        error = 360 - currYaw + tYaw;

    } else {
        error = tYaw - currYaw;
    }

    return error;
}


void takeOff(void* pvParameters) {
    uint8_t target = 0;
    const uint16_t delay_ms = 1000/CONTROLLER_RATE_HZ;
    xTakeOffSemaphore = xSemaphoreCreateBinary();
    int n = 0;

    xSemaphoreTake(xTakeOffSemaphore, portMAX_DELAY);
    while(1) {

        if (!GPIOPinRead(REF_GPIO_BASE, REF_PIN)) {
            reference = getYaw();
            xSemaphoreGive(xControlSemaphore);
            xSemaphoreGive(xButtPollSemaphore);
            xSemaphoreTake(xTakeOffSemaphore, portMAX_DELAY);
        } else {
            if (n >= 250/2) {
                target+= 5;
                n =0;
            }

            setMotor(MOTOR_T, 10);
            piTailUpdate(target);
            n++;
        }

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}


void piMainUpdate(uint8_t setAlt) {
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

    if (counter ==  2000) {
        //dprintf ("CAlt %d \n", getAlt());
        counter = 0;
    }
}


void piTailUpdate(int16_t setYaw) {

    static double I;
    double P;
    double control;
    int16_t error_yaw;
    double dI;

    error_yaw = getYawErr(setYaw); // Error between the set altitude and the actual altitude


    P = KP_T*error_yaw;

    dI = KI_T*error_yaw*T_DELTA;

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

    if (counter == 20) {
        //dprintf ("CYaw %d \n", getYaw());
        counter = 0;
    }
    counter++;


}
