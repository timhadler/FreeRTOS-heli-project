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

#include "buttons4.h"


static int16_t refYaw;
static uint8_t state;
static bool foundRef;

static uint8_t targetAlt;
static int16_t targetYaw;

static bool mode1_flag;
static bool mode2_flag;


void setMode1(void) {
    mode1_flag = true;
}


void setMode2(void) {
    mode2_flag = true;
}



int16_t getRefYaw(void) {
    return refYaw;
}


uint8_t getState(void) {
    return state;
}


uint8_t getTargetAlt(void) {
    return targetAlt;
}


int16_t getTargetYaw(void) {
    return targetYaw;
}


void incAlt(void) {
    if (targetAlt != 100) {
        targetAlt += 10;
    }
}


void decAlt(void) {
    if (targetAlt != 0) {
        targetAlt -= 10;
    }
}


void incYaw(void) {
    if (targetYaw == 345) {
        targetYaw = 0;
    } else {
        targetYaw += 15;
    }
}


void decYaw(void) {
    if (targetYaw == 0) {
        targetYaw = 345;
    } else {
        targetYaw -= 15;
    }
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
    //int16_t yaw = 0;

    xTakeOffSemaphore = xSemaphoreCreateBinary();
    xLandSemaphore = xSemaphoreCreateBinary();
    //uint8_t count = 0;
    while(1) {
        switch(state) {
            case LANDED:
                //xSemaphoreTake(xTakeOffSemaphore, portMAX_DELAY);
                state = TAKE_OFF;
                break;

            case TAKE_OFF:
                //yaw = getYaw();
                //if (foundRef && (yaw <= 5 || yaw >= 355) && getAlt() > 8) {
                    state = IN_FLIGHT;
                    xSemaphoreGive(xButtPollSemaphore);
                //}
                break;

            case IN_FLIGHT:
                xSemaphoreTake(xLandSemaphore, portMAX_DELAY);
                state = LANDING;
                break;

            case LANDING:
                if (getAlt == 0) {
                    //count++;
                    state = LANDED;
                }
                // If at 0 alt for half a second
/*                if (count >= 40) {
                    count = 0;
                    state = LANDED;
                }*/
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}


void mode2 (void) {
    static bool clockwise = true;
    static int n = 0;
    static int16_t tYawRef = 500;
    int16_t cYaw = getYaw();

    if (tYawRef == 500) {
        tYawRef = targetYaw;

    } else {
        if (clockwise) {
            targetYaw = tYawRef + 30;
        } else {
            targetYaw = tYawRef - 30;
        }
        if (cYaw < (targetYaw + 5) && cYaw > (targetYaw - 5)) {
            if (n < 4) {
                clockwise =!clockwise;
                n++;
            } else {
                mode2_flag = false;
                targetYaw = tYawRef;
                tYawRef = 500;
                n = 0;
            }
        }
    }
}


void controller(void* pvParameters) {
    //xControlSemaphore = xSemaphoreCreateBinary();
    const uint16_t delay_ms = 1000/CONTROLLER_RATE_HZ;
    int16_t yaw = 0;
    //xSemaphoreTake(xControlSemaphore, portMAX_DELAY);
    int16_t tYaw = 500;
    foundRef = false;
    uint16_t tick = 0;
    while(1) {
        tick++;

        if (state == TAKE_OFF) {
            // If reference not found, find it then take off
            if (!foundRef) {
                if (!GPIOPinRead(REF_GPIO_BASE, REF_PIN)) {
                    // Ref found
                    setYawReference();
                    targetYaw = 0;
                    //targetAlt = 10;
                    foundRef = true;

                  // If heli is not facing reference, increment target yaw at a fixed rate
                } else if (tick >= CONTROLLER_RATE_HZ / UPDATE_TARGET_RATE_HZ){
                    targetYaw = getYaw() + 5;
                    targetAlt = 10;
                    tick = 0;
                }
            } else {
                // Ref already found
                targetYaw = 0;
                targetAlt = 10;
            }

        } else if (state == LANDING) {
            // Rotate to yaw reference then decrease target alt at a fixed rate
            targetYaw = 0;//getRefYaw();
            yaw = getYaw();
            if ((yaw < 5 || yaw > 355) && tick >= CONTROLLER_RATE_HZ / 1) {
                if (targetAlt >= 10) {
                    targetAlt -= 10;
                    tick = 0;
                }
            }
        } else if (state == IN_FLIGHT) {
            if (mode1_flag) {
                yaw = getYaw();
                if (tYaw == 500) {
                    if (targetYaw < 180) {
                        tYaw = targetYaw + 180;
                        //tYaw = tYaw - tYaw % 15;
                    } else {
                        tYaw = targetYaw - 180;
                        //tYaw = tYaw - tYaw % 15;
                    }
                } else if (tick >= CONTROLLER_RATE_HZ/1) {
                    tick = 0;
                    if (targetYaw < tYaw) {
                        //targetYaw = yaw + 15;
                        //incYaw();
                        targetYaw = tYaw;
                    } else if (targetYaw > tYaw) {
                        //decYaw();
                        targetYaw = tYaw;
                    } else {
                        mode1_flag = false;
                        tYaw = 500;
                    }
                }
            } else if (mode2_flag) {
                mode2();
            }
        }

        piMainUpdate();
        piTailUpdate();

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}



void piMainUpdate(void) {
    int control;
    int error;
    static int dI;
    int P;

    error = getAltErr(targetAlt); // Error between the set altitude and the actual altitude
    dI += error*T_DELTA * 1000;

    P = CLAMP(KP_M*error, -MAXIMUM_P_CONTROL, MAXIMUM_P_CONTROL);
    //I = CLAMP(KI_M*error, -MAXIMUM_I_CONTROL, MAXIMUM_I_CONTROL);
    control = P + KI_M*dI / 1000;

    // Enforces output limits
    if (control > OUTPUT_MAX) {
        control = OUTPUT_MAX;
    } else if (control < OUTPUT_MIN) {
        control = OUTPUT_MIN;
    }

    setMotor(MOTOR_M, control);
}


void piTailUpdate(void) {
    int control;
    int error;
    static int dI;
    int P;
    error = getYawErr(targetYaw); // Error between the set altitude and the actual altitude
    dI += error * T_DELTA * 1000;

    P = CLAMP(KP_T*error, -MAXIMUM_P_CONTROL, MAXIMUM_P_CONTROL);
    //I = CLAMP(KI_T*dI/1000, -MAXIMUM_I_CONTROL, MAXIMUM_I_CONTROL);
    control = P + KI_T*dI/1000;

    // Enforces output limits
    if (control > OUTPUT_MAX) {
        control = OUTPUT_MAX;
    } else if (control < OUTPUT_MIN) {
        control = OUTPUT_MIN;
    }
    setMotor(MOTOR_T, control);
}
