/*
 * user_input.h
 *
 *  Created on: 18/08/2020
 *      Author: tch118
 */

#ifndef USER_INPUT_H_
#define USER_INPUT_H_

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "buttons4.h"
#include "controllers.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

//******************************************************************
// Macros -  Define MODE switch
//******************************************************************
#define MODE_PERIPH_GPIO            SYSCTL_PERIPH_GPIOA
#define SWITCH_MODE_GPIO_BASE       GPIO_PORTA_BASE
#define SWITCH_MODE_INT_PIN         GPIO_INT_PIN_7
#define SWITCH_MODE_PIN             GPIO_PIN_7
#define BUTTON_POLL_RATE_HZ         60
#define TIME_THRESHOLD_MS           500
#define SEQUENCE_LENGTH             4
#define SEQUENCE_1                  1
#define SEQUENCE_2                  2

//******************************************************************
// Global Variables
//******************************************************************
static uint8_t sequence_1[SEQUENCE_LENGTH] = {UP, UP, DOWN, DOWN};
static uint8_t sequence_2[SEQUENCE_LENGTH] = {DOWN, DOWN, UP, UP};
static uint8_t seq1_pos;
static uint8_t seq2_pos;
enum buttStates {IDLE=0, ST_1, ST_2, ST_3, ST_4};
static uint8_t state;

void initModeSwitch(void);

void SwitchModeIntHandler(void);

void pollButtons(void* pvParameters);

#endif /* USER_INPUT_H_ */
