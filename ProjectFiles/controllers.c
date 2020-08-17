/*
 * controllers.c
 *
 *  Created on: 7/08/2020
 *      Author: tch118
 */

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "controllers.h"
#include "myMotors.h"
#include "altitude.h"
#include "myYaw.h"


static int16_t refYaw;
static uint8_t state;
static bool foundRef;


int16_t getRefYaw(void) {
    return refYaw;
}


void setRefYaw(int16_t ref) {
/*    refYaw = ref;*/
    foundRef = true;
}


uint8_t getState(void) {
    return state;
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


void FSM(void* pvParameters){
    //state = LANDED;
    const uint16_t delay_ms = 1000/CONTROLLER_RATE_HZ;
    int16_t yaw = 0;

    xTakeOffSemaphore = xSemaphoreCreateBinary();
    xLandSemaphore = xSemaphoreCreateBinary();
    uint8_t count = 0;
    while(1) {
        switch(state) {
            case LANDED:
                xSemaphoreTake(xTakeOffSemaphore, portMAX_DELAY);
                state = TAKE_OFF;
                break;

            case TAKE_OFF:
                yaw = getYaw();
                if (foundRef && (yaw <= 5 || yaw >= 355) && getAlt() > 8) {
                    state = IN_FLIGHT;
                    xSemaphoreGive(xButtPollSemaphore);
                }
                break;

            case IN_FLIGHT:
                xSemaphoreTake(xLandSemaphore, portMAX_DELAY);
                state = LANDING;
                break;

            case LANDING:
                if (getAlt == 0) {
                    count++;
                }
                // If at 0 alt for half a second
                if (count >= 40) {
                    count = 0;
                    state = LANDED;
                }
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}


void takeOff(void* pvParameters) {
    uint8_t target = getYaw();
    const uint16_t delay_ms = 1000/CONTROLLER_RATE_HZ;
    //xTakeOffSemaphore = xSemaphoreCreateBinary();
    int n = 0;

    //xSemaphoreTake(xTakeOffSemaphore, portMAX_DELAY);
    while(1) {
/*        if (state == IN_FLIGHT) {
            xSemaphoreTake(xTakeOffSemaphore, portMAX_DELAY);
        }*/
        //state = TAKE_OFF;

        if (!GPIOPinRead(REF_GPIO_BASE, REF_PIN)) {
            refYaw = getYaw();
            break;
            //xSemaphoreGive(xFSMSemaphore);
            //xSemaphoreTake(xTakeOffSemaphore, portMAX_DELAY);
            //state = IN_FLIGHT;
            //xSemaphoreGive(xControlSemaphore);
            //xSemaphoreGive(xButtPollSemaphore);
            //xSemaphoreTake(xTakeOffSemaphore, portMAX_DELAY);
            //xSemaphoreTake(xControlSemaphore, portMAX_DELAY);
            //xSemaphoreTake(xButtPollSemaphore, portMAX_DELAY);
        } else {
            if (n >= 250/2) {
                target+= 5;
                n =0;
            }

            setMotor(MOTOR_T, 15);
            piTailUpdate(target);
            n++;
        }

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}


void land(void* pvParameters) {
    uint8_t height = 0;
    //xLandSemaphore = xSemaphoreCreateBinary();

    //xSemaphoreTake(xLandSemaphore, portMAX_DELAY);
    //xSemaphoreTake(xButtPollSemaphore, portMAX_DELAY);
    //xSemaphoreTake(xControlSemaphore, portMAX_DELAY);
    while(1) {
        //state = LANDING;
        height = getAlt();
        if (height > 0) {
            if (GPIOPinRead(REF_GPIO_BASE, REF_PIN) == 0) {
                height = 0;
            } else if (height > 25){
                height = 25;
            }
            piMainUpdate(height);
            piTailUpdate(refYaw);

        } else {
            //state = LANDED;
            //xSemaphoreGive(xButtPollSemaphore);
            //xSemaphoreGive(xControlSemaphore);
            //xSemaphoreGive(xFSMSemaphore);
            //xSemaphoreTake(xLandSemaphore, portMAX_DELAY);
            break;
        }
    }
}


void piMainUpdate(uint8_t setAlt) {
    int control;
    int error;
    static int dI;
    static int lastTarget = 0;

    if (lastTarget != setAlt) {
        dI = 0;
    }

    error = getAltErr(setAlt); // Error between the set altitude and the actual altitude
    dI += error*T_DELTA * 1000;

    control = KP_M*error + KI_M*dI / 1000;

    // Enforces output limits
    if (control > OUTPUT_MAX) {
        control = OUTPUT_MAX;
    } else if (control < OUTPUT_MIN) {
        control = OUTPUT_MIN;
    }

    lastTarget = setAlt;
    setMotor(MOTOR_M, control);
}


void piTailUpdate(int16_t setYaw) {
    int control;
    int error;
    static int dI;
    static int lastTarget = 0;

    if (lastTarget != setYaw) {
        dI = 0;
    }

    error = getYawErr(setYaw); // Error between the set altitude and the actual altitude
    dI += error * T_DELTA * 1000;

    control = KP_T*error + KI_T*dI / 1000;

    // Enforces output limits
    if (control > OUTPUT_MAX) {
        control = OUTPUT_MAX;
    } else if (control < OUTPUT_MIN) {
        control = OUTPUT_MIN;
    }
    lastTarget = setYaw;
    setMotor(MOTOR_T, control);
}
