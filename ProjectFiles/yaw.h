/*
 *  yaw.h
 *
 *  Contributers: Hassan Alhujhoj, Abdullah Naeem and Tim Hadler
 *  Created on: 6/08/2020
 */

#ifndef MYYAW_H_
#define MYYAW_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"

#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"

//******************************************************************
// Macros - Define YAW GPIO base and pins
//******************************************************************
#define CH_A                GPIO_PIN_0      // CHannel A and B for sensing yaw
#define CH_B                GPIO_PIN_1
#define YAW_PERIPH_GPIO     SYSCTL_PERIPH_GPIOB
#define YAW_GPIO_BASE       GPIO_PORTB_BASE
#define YAW_CHA_INT_PIN     GPIO_INT_PIN_0
#define YAW_CHB_INT_PIN     GPIO_INT_PIN_1
#define DISK_INTERRUPTS     448             // The number of readings in the slotted disk (4 * (112 slots) in a rev)
#define QUEUE_SIZE          1               // Matches the number of samples per period of jitter, ensuring it will not significantly deviate

// Yaw Reference signal pin
#define REF_PERIPH              SYSCTL_PERIPH_GPIOC
#define REF_GPIO_BASE           GPIO_PORTC_BASE
#define REF_INT_PIN             GPIO_INT_PIN_4
#define REF_PIN                 GPIO_PIN_4

//******************************************************************
// Global Variables
//******************************************************************
enum quadrature {A=0, B=1, C=3, D=2};       // Sets the values for the finite state machine
static int32_t slots;

int32_t getYaw(void);

void initYaw(void);

void setYawReference(void);

void YawIntHandler(void);

#endif /* MYYAW_H_ */
