/*
 * myYaw.h
 *
 *  Created on: 6/08/2020
 *      Author: tch118
 */

#ifndef MYYAW_H_
#define MYYAW_H_

#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"


// define Yaw gpio base and pins
#define CH_A   GPIO_PIN_0 // CHannel A and B for sensing yaw
#define CH_B   GPIO_PIN_1
#define YAW_PERIPH_GPIO SYSCTL_PERIPH_GPIOB
#define YAW_GPIO_BASE GPIO_PORTB_BASE
#define YAW_CHA_INT_PIN GPIO_INT_PIN_0
#define YAW_CHB_INT_PIN GPIO_INT_PIN_1
#define DISK_INTERRUPTS 448 // The number of readings in the slotted disk (4 * (112 slots) in a rev)

/* Sets variables */

/* FreeRTOS variables*/
//static QueueHandle_t YAW_Queue;
//static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

void
initYaw(void);

void
setYawReference(void);

int32_t
getYaw(void);

void
YawIntHandler(void);

#endif /* MYYAW_H_ */
