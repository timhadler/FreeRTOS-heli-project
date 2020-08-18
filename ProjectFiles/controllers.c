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
#include "userInput.h"


static uint8_t state;
static bool foundRef;

static uint8_t targetAlt;
static int16_t targetYaw;

static bool mode1_flag;
static bool mode2_flag;


uint8_t getState(void) {
    return state;
}


uint8_t getTargetAlt(void) {
    return targetAlt;
}


int16_t getTargetYaw(void) {
    return targetYaw;
}


void setMode1(void) {
    mode1_flag = true;
}


void setMode2(void) {
    mode2_flag = true;
}


int16_t getAltErr(void) {
    uint8_t tAlt = targetAlt;
    return tAlt - getAlt();
}


int16_t getYawErr(void) {
    int16_t error = 0;
    int16_t tYaw = targetYaw;
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


void FSM(void* pvParameters){
    //state = LANDED;
    const uint16_t delay_ms = 1000/CONTROLLER_RATE_HZ;
    int16_t yaw = 0;

    xTakeOffSemaphore = xSemaphoreCreateBinary();
    xLandSemaphore = xSemaphoreCreateBinary();
    //static uint8_t count = 0;
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
                if (getAlt() == 0) {
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


void takeOff(uint16_t* timer) {
    // If reference not found, find it then take off
    if (!foundRef) {
        if (!GPIOPinRead(REF_GPIO_BASE, REF_PIN)) {
            // Ref found
            setYawReference();
            targetYaw = 0;
            //targetAlt = 10;
            foundRef = true;

          // If heli is not facing reference, increment target yaw at a fixed rate
        } else if (*timer >= CONTROLLER_RATE_HZ / UPDATE_TARGET_RATE_HZ){
            targetYaw = getYaw() + 15;
            targetAlt = 10;
            *timer = 0;
        }
    } else {
        // Ref already found
        targetYaw = 0;
        targetAlt = 10;
    }
}


void land(uint16_t* timer) {
    // Rotate to yaw reference then decrease target alt at a fixed rate
    targetYaw = 0;
    int16_t yaw = getYaw();
    if ((yaw < 5 || yaw > 355) && *timer >= CONTROLLER_RATE_HZ / 1) {
        if (targetAlt >= 10) {
            targetAlt -= 10;
            *timer = 0;
        }
    }
}


void inFlight(uint16_t* timer) {
    int16_t tYaw = 0;
    int16_t cYaw;
    if (mode1_flag) {
        // Do mode 1

/*        if (tYaw == 0) {
            //180
            cYaw = getYaw();
            if (cYaw < 180) {
                tYaw = 180 + cYaw;
            } else {
                tYaw = cYaw - 180;
            }
        }
        targetYaw = tYaw;*/
    }
}


void controller(void* pvParameters) {
    const uint16_t delay_ms = 1000/CONTROLLER_RATE_HZ;

    uint16_t tick = 0;
    while(1) {
        tick++;

        if (state == TAKE_OFF) {
            takeOff(&tick);

        } else if (state == LANDING) {
            land(&tick);
        } else if (state == IN_FLIGHT) {
            inFlight(&tick);
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
    static int lastTarget = 0;
    int tAlt = targetAlt;

    if (lastTarget != tAlt) {
        //dI = 0;
    }

    error = getAltErr(); // Error between the set altitude and the actual altitude
    dI += error*T_DELTA * 1000;

    control = KP_M*error + KI_M*dI / 1000;

    // Enforces output limits
    if (control > OUTPUT_MAX) {
        control = OUTPUT_MAX;
    } else if (control < OUTPUT_MIN) {
        control = OUTPUT_MIN;
    }

    lastTarget = tAlt;
    setMotor(MOTOR_M, control);
}


void piTailUpdate(void) {
    int control;
    int error;
    static int dI;
    static int lastTarget = 0;
    int tYaw = targetYaw;

    if (lastTarget != tYaw) {
        dI = 0;
    }

    error = getYawErr(); // Error between the set altitude and the actual altitude
    dI += error * T_DELTA * 1000;

    control = KP_T*error + KI_T*dI / 1000;

    // Enforces output limits
    if (control > OUTPUT_MAX) {
        control = OUTPUT_MAX;
    } else if (control < OUTPUT_MIN) {
        control = OUTPUT_MIN;
    }
    lastTarget = tYaw;
    setMotor(MOTOR_T, control);
}
