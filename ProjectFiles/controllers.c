/*
 * controllers.c
 *
 * Module for controlling the heli motors
 *
 * Contributers: Hassan Alhujhoj, Abdullah Naeem and Tim Hadler
 * Created on: 7/08/2020
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "controllers.h"
#include "motors.h"
#include "altitude.h"
#include "yaw.h"


//******************************************************************
// Global variables
//******************************************************************
static uint8_t state;
static uint8_t targetAlt;
static int16_t targetYaw;
static bool foundRef;

// Flags to trigger special flight modes
static bool mode1_flag;     // 180 deg turn
static bool mode2_flag;     // 'Shake head' mode


//  sets the flag for mode 1
void setMode1(void) {
    mode1_flag = true;
}


// sets the flag for mode 2
void setMode2(void) {
    mode2_flag = true;
}


// Returns the current heli state
uint8_t getState(void) {
    return state;
}


// returns the target altitude
uint8_t getTargetAlt(void) {
    return targetAlt;
}


// returns the target yaw
int16_t getTargetYaw(void) {
    return targetYaw;
}


// increases target altitude
void incAlt(void) {
    if (targetAlt != 100)
    {
        targetAlt += 10;
    }
}


// decreases target altitude
void decAlt(void) {
    if (targetAlt != 0)
    {
        targetAlt -= 10;
    }
}


// increases target yaw
void incYaw(void) {
    if (targetYaw == 345)
    {
        targetYaw = 0;
    } else {
        targetYaw += 15;
    }
}


// decreases target yaw
void decYaw(void) {
    if (targetYaw == 0)
    {
        targetYaw = 345;
    } else
    {
        targetYaw -= 15;
    }
}


// returns the current altitude error
int16_t getAltErr(void) {
    return targetAlt - getAlt();
}


// returns current yaw error
int16_t getYawErr(void) {
    int16_t error = 0;
    int16_t currYaw = getYaw();
    int16_t tYaw = targetYaw;

    // Calculates error for when heli is within 0-90 quadrant, and target yaw in 260-360,
    if (tYaw > 260 && currYaw < 90)
    {
        error = tYaw - 360 - currYaw;

        // Calculates error for when heli is within 260-360 quadrant, and target yaw in 0-90
    } else if (tYaw < 90 && currYaw > 260)
    {
        error = 360 - currYaw + tYaw;

    } else
    {
        error = tYaw - currYaw;
    }
    return error;
}


/* FSM controls state transitions
 * When in Landed or IN FLIGHT state, this task is blocked by semaphores given from
 * the mode switch interrupt handler, and state switches once the semaphores have been given
 * When in Landing or TAKE oFF mode, the state switches when the landed and take off conditions are met
 */
void FSM(void* pvParameters)
{
    const uint16_t delay_ms = 1000/CONTROLLER_RATE_HZ;
    int16_t yaw = 0;

    xTakeOffSemaphore = xSemaphoreCreateBinary();
    xLandSemaphore = xSemaphoreCreateBinary();

    while(1)
    {
        switch(state)
        {
            case LANDED:
                // Block task until take off signal from mode switch
                xSemaphoreTake(xTakeOffSemaphore, portMAX_DELAY);
                state = TAKE_OFF;
                break;

            case TAKE_OFF:
                // Wait for heli to be facing yaw reference and at 10% altitude before advancing state
                yaw = getYaw();
                if (foundRef && WITHIN_5_DEGREES && getAlt() > 10)
                {
                    state = IN_FLIGHT;
                    // Unblock pollButtons task when in flight
                    xSemaphoreGive(xButtPollSemaphore);
                }
                break;

            case IN_FLIGHT:
                // Block task until land signal from mode switch
                xSemaphoreTake(xLandSemaphore, portMAX_DELAY);
                state = LANDING;
                break;

            case LANDING:
                // Wait for heli to be landed before advancing state
                if (getAlt() == 0)
                {
                    state = LANDED;
                }
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}


/* Special mode 1 is a one eighty turn
 * Sets the target Yaw to 180 degrees from current yaw
 */
void oneEighty(void) {
    int16_t yaw = getYaw();
    static int16_t tYaw = UNASSIGNED;

    if (tYaw == UNASSIGNED)
    {
        // Assumes heli is facing the current target yaw, Find 180 yaw from current targetYaw
        if (targetYaw < 180)
        {
            tYaw = targetYaw + 180;
        } else {
            tYaw = targetYaw - 180;
        }

    } else
    {
        targetYaw = tYaw;
        // reset tYaw to unassigned so we can do a 180 again from a different angle
        tYaw = UNASSIGNED;
        mode1_flag = false;
    }
}


/* Mode 2 is a heli head shake
 * Rotates heli +- 15 degress from current yaw
 */
void headShake (void)
{
    static bool clockwise = true;
    static int n = 0;
    static int16_t tYawRef = UNASSIGNED;
    int16_t cYaw = getYaw();

    if (tYawRef == UNASSIGNED)
    {
        tYawRef = targetYaw;

    } else
    {
        if (clockwise)
        {
            targetYaw = tYawRef + 15;
        } else
        {
            targetYaw = tYawRef - 15;
        }
        // If it is within 5 degrees of current targetYaw, can switch direction
        if (cYaw < (targetYaw + 5) && cYaw > (targetYaw - 5))
        {
            if (n < NUM_SHAKES)
            {
                clockwise =!clockwise;
                n++;
            } else
            {
                // Once heli has shaken NUM_SHAKES times, reset mode
                mode2_flag = false;
                targetYaw = tYawRef;
                tYawRef = UNASSIGNED;
                n = 0;
            }
        }
    }
}


/* Controller task, initiates Main and Tail controllers
 * sets targetYaw and targetHeight based on the current state
 */
void controller(void* pvParameters)
{
    int16_t yaw = 0;
    uint16_t tick = 0;
    foundRef = false;
    const uint16_t delay_ms = 1000/CONTROLLER_RATE_HZ;

    while(1)
    {
        // Tick increments approximately at CONTROLLER_RATE_HZ
        tick++;

        if (state == TAKE_OFF)
        {
            // If reference not found, find it then take off
            if (!foundRef) {
                if (!GPIOPinRead(REF_GPIO_BASE, REF_PIN))
                {
                    // Ref found
                    setYawReference();       // 0 degrees now corresponds to the yaw reference
                    targetYaw = 0;
                    foundRef = true;

                // If heli is not facing reference, increment target yaw at a fixed rate
                } else if (tick >= CONTROLLER_RATE_HZ / UPDATE_TARGET_RATE_HZ)
                {
                    targetYaw = getYaw() + 5;
                    targetAlt = 10;
                    tick = 0;
                }
            } else
            {
                // Ref already found
                targetYaw = 0;
                targetAlt = 10;
            }

        } else if (state == LANDING)
        {
            // Rotate to yaw reference then decrease target alt at a fixed rate
            targetYaw = 0;
            yaw = getYaw();
            // Decrese target altitude if heli is within 5 degrees of reference
            if (WITHIN_5_DEGREES && tick >= CONTROLLER_RATE_HZ / 1)
            {
                if (targetAlt >= 10)
                {
                    targetAlt -= 10;
                    tick = 0;
                }
            }
        } else if (state == IN_FLIGHT)
        {
            if (mode1_flag)
            {
                // Perform mode 1
                oneEighty();
            } else if (mode2_flag)
            {
                // Perform mode 2
                headShake();
            }
        }

        // Update motor control
        piMainUpdate();
        piTailUpdate();

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}


// PI controller for the main rotor
void piMainUpdate(void) {
    int P;
    int I;
    int control;
    int error = 0;
    static int dI;

    error = getAltErr(); // Error between the set altitude and the actual altitude
    dI += error*T_DELTA * 1000;

    // Limits the output control
    P = CLAMP(KP_M*error, -MAXIMUM_P_MAIN_CONTROL, MAXIMUM_P_MAIN_CONTROL);
    I = CLAMP(KI_M*dI/1000, -MAXIMUM_I_MAIN_CONTROL, MAXIMUM_P_MAIN_CONTROL);

    control = P + I;

    // Enforces output limits
    if (control > OUTPUT_MAX)
    {
        control = OUTPUT_MAX;
    } else if (control < OUTPUT_MIN)
    {
        control = OUTPUT_MIN;
    }

    setMotor(MOTOR_M, control);
}


// PI controller for the tail rotor
void piTailUpdate(void) {
    int P;
    int I;
    int error;
    int control;
    static int dI;

    error = getYawErr(); // Error between the set altitude and the actual altitude
    dI += error * T_DELTA * 1000;

    // Limits the output control
    P = CLAMP(KP_T*error, -MAXIMUM_P_TAIL_CONTROL, MAXIMUM_P_TAIL_CONTROL);
    I = CLAMP(KI_T*dI/1000, -MAXIMUM_I_TAIL_CONTROL, MAXIMUM_I_TAIL_CONTROL);

    control = P + I;

    // Enforces output limits
    if (control > OUTPUT_MAX)
    {
        control = OUTPUT_MAX;
    } else if (control < OUTPUT_MIN)
    {
        control = OUTPUT_MIN;
    }
    setMotor(MOTOR_T, control);
}
