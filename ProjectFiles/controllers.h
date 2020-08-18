/*
 * controllers.h
 *
 *  Created on: 7/08/2020
 *      Author: tch118,
 *
 */

#ifndef CONTROLLERS_H_
#define CONTROLLERS_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"


// Define ref signal base and pin
#define REF_PERIPH SYSCTL_PERIPH_GPIOC
#define REF_GPIO_BASE GPIO_PORTC_BASE
#define REF_INT_PIN GPIO_INT_PIN_4
#define REF_PIN GPIO_PIN_4

#define KP_M 1
#define KI_M 0.5
#define KP_T 2
#define KI_T 1
#define T_DELTA 0.004
#define OUTPUT_MAX 95
#define OUTPUT_MIN 5
#define CONTROLLER_RATE_HZ 250
#define UPDATE_TARGET_RATE_HZ 3


enum heliStates {LANDED=0, LANDING, TAKE_OFF, IN_FLIGHT};

SemaphoreHandle_t xButtPollSemaphore;
SemaphoreHandle_t xFSMSemaphore;
SemaphoreHandle_t xTakeOffSemaphore;
SemaphoreHandle_t xLandSemaphore;

void
initControllers(void);

uint8_t getState(void);

int16_t
getAltErr(void);

int16_t
getYawErr(void);

uint8_t
getTargetAlt(void);

int16_t
getTargetYaw(void);

void
incAlt(void);

void
decAlt(void);

void
incYaw(void);

void
decYaw(void);

void
piMainUpdate(void);

void
piTailUpdate(void);

void
FSM(void* pvParameters);

void
controller(void* pvParameters);

#endif /* CONTROLLERS_H_ */
