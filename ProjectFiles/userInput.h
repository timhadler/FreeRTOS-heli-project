/*
 * user_input.h
 *
 *  Created on: 18/08/2020
 *      Author: tch118
 */

#ifndef USER_INPUT_H_
#define USER_INPUT_H_

#include "buttons4.h"

//******************************************************************
// Macros
//******************************************************************
// For the mode switch
#define MODE_PERIPH_GPIO            SYSCTL_PERIPH_GPIOA
#define SWITCH_MODE_GPIO_BASE       GPIO_PORTA_BASE
#define SWITCH_MODE_INT_PIN         GPIO_INT_PIN_7
#define SWITCH_MODE_PIN             GPIO_PIN_7
#define BUTTON_POLL_RATE_HZ         60
#define TIME_THRESHOLD_MS           500
#define SEQUENCE_LENGTH             4
#define SEQUENCE_1                  1
#define SEQUENCE_2                  2

// For unblocking right button
#define GPIO_PORTF_LOCK_R       (*((volatile uint32_t *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile uint32_t *)0x40025524))
#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define GPIO_LOCK_M             0xFFFFFFFF  // GPIO Lock

//******************************************************************
// Global Variables
//******************************************************************
static uint8_t sequence_1[SEQUENCE_LENGTH] = {UP, UP, DOWN, DOWN};
static uint8_t sequence_2[SEQUENCE_LENGTH] = {DOWN, DOWN, UP, UP};
static uint8_t seq1_pos;
static uint8_t seq2_pos;

void initUserInput(void);

void SwitchModeIntHandler(void);

void pollButtons(void* pvParameters);

#endif /* USER_INPUT_H_ */
