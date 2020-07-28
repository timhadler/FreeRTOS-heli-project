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


//*****************************
// Values here may need to be revised
//*****************************
#define BUF_SIZE 10     // Circular buffer size
#define SAMPLE_RATE_HZ 253
#define BUTT_POLLING_RATE_HZ 50
#define DISPLAY_RATE 60
#define MAX 962 // Calibrated for a 0.8V drop (Mean ADC drop of ~= 962)


// define Yaw gpio base and pins
#define CH_A   GPIO_PIN_0 // CHannel A and B for sensing yaw
#define CH_B   GPIO_PIN_1
#define YAW_PERIPH_GPIO SYSCTL_PERIPH_GPIOB
#define YAW_GPIO_BASE GPIO_PORTB_BASE
#define YAW_CHA_INT_PIN GPIO_INT_PIN_0
#define YAW_CHB_INT_PIN GPIO_INT_PIN_1

// Define MODE switch
#define MODE_PERIPH_GPIO SYSCTL_PERIPH_GPIOA
#define SWITCH_MODE_GPIO_BASE GPIO_PORTA_BASE
#define SWITCH_MODE_INT_PIN GPIO_INT_PIN_7
#define SWITCH_MODE_PIN GPIO_PIN_7

// Define ref signal base and pin
#define REF_PERIPH SYSCTL_PERIPH_GPIOC
#define REF_GPIO_BASE GPIO_PORTC_BASE
#define REF_INT_PIN GPIO_INT_PIN_4
#define REF_PIN GPIO_PIN_4


#endif /* CONSTANTS_H_ */
