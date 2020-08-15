/*
 * constants.h
 *
 * Defines constants used in the main source file for ENCE464 Heli project
 *
 *  Created on: 28/07/2020
 *      Author: tch118
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_


// Define UART constants
#define MAX_STR_LEN 16
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX



#define DISPLAY_RATE 60
#define MAX_HEIGHT 962 // Calibrated for a 0.8V drop (Mean ADC drop of ~= 962)


// LED constants
#define LED_GPIO_BASE GPIO_PORTF_BASE
#define LED_RED_PIN GPIO_PIN_1
#define LED_BLUE_PIN GPIO_PIN_2
#define LED_GREEN_PIN GPIO_PIN_3


// For unlcoking rght button
#define GPIO_PORTF_LOCK_R       (*((volatile uint32_t *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile uint32_t *)0x40025524))
#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define GPIO_LOCK_M             0xFFFFFFFF  // GPIO Lock


#endif /* CONSTANTS_H_ */
