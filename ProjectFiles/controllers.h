/*
 * controllers.h
 *
 *  Created on: 7/08/2020
 *      Author: tch118
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



void initControllers(void);

int32_t getAltErr(int32_t cMean);

int32_t getYawErr(int16_t cYaw);

void updateControl(int32_t altError, int32_t yawError);

void findReference(void);

#endif /* CONTROLLERS_H_ */
