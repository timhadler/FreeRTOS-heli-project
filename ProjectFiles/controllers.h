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
#define KI_M 0.3
#define KP_T 0.7
#define KI_T 0.3
#define T_DELTA 0.004
#define OUTPUT_MAX 95
#define OUTPUT_MIN 5


void
initControllers(void);

int16_t
getAltErr(int16_t setAlt);

int16_t
getYawErr(int16_t setAlt);

void
piMainUpdate(uint8_t setAlt);

void
piTailUpdate(int16_t setYaw);

uint8_t
takeOff(void);

#endif /* CONTROLLERS_H_ */
