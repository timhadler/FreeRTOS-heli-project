#ifndef DEBUGGER_H_
#define DEBUGGER_H_

/*
 * debugger.h
 *
 * Sends selected variables through a serial interface for debugging purposes.
 * The variables include the altitude, YAW, helicopter state and PWM.
 *
 *  Created on: 27/07/2020
 *  Contributers: Hassan Alhujhoj, Abdullah Naeem and Tim Hadler
 *  Last modified: 08/08/2020
 */


#include <stdint.h>


//******************************************************************
// Macros - Define UART constants
//******************************************************************
#define MAX_STR_LEN             16
#define BAUD_RATE               9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX
#define UART_SEND_RATE_HZ       4


//******************************************************************
// Function declararions
//******************************************************************
// initialiseUSB_UART - 8 bits, 1 stop bit, no parity
void initialiseUSB_UART (void);

//Converts the landed state to a string for display
char* stateToString (uint8_t heliState);

// Updates the UART communication
void sendData (void* pvParameters);

// Transmit a string via UART0
void UARTSend (char *pucBuffer);

#endif /*DEBUGGER_H_*/
