/*
 * constants.h
 *
 * Defines constants used in the main source file for ENCE464 Heli project
 *
 *  Contributers: Hassan Alhujhoj, Abdullah Naeem and Tim Hadler
 *  Created on: 28/07/2020
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_



#define DISPLAY_RATE            60


// Define MODE switch
#define MODE_PERIPH_GPIO        SYSCTL_PERIPH_GPIOA
#define SWITCH_MODE_GPIO_BASE   GPIO_PORTA_BASE
#define SWITCH_MODE_INT_PIN     GPIO_INT_PIN_7
#define SWITCH_MODE_PIN         GPIO_PIN_7

// For unblocking right button
#define GPIO_PORTF_LOCK_R       (*((volatile uint32_t *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile uint32_t *)0x40025524))
#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define GPIO_LOCK_M             0xFFFFFFFF  // GPIO Lock

#endif /* CONSTANTS_H_ */
