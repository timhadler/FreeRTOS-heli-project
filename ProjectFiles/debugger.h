#ifndef DEBUGGER_H_
#define DEBUGGER_H_

/*
 * debugger.h
 *
 * Sends selected variables through a serial interface for debugging purposes.
 * The variables include the altitude, yaw, helicopter state and PWM.
 *
 *  Created on: 27/07/2020
 *  Contributers: Hassan Alhujhoj, Abdullah Naeem and Tim Hadler
 *  Last modified: 8.8.2020
 */


#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>




// initialiseUSB_UART - 8 bits, 1 stop bit, no parity
void initialiseUSB_UART (void);

// initialiseUSB_UART - 8 bits, 1 stop bit, no parity
char* stateToString (uint8_t heliState);

// Updates the UART communication
void sendData (void* pvParameters);

// Transmit a string via UART0
void UARTSend (char *pucBuffer);

#endif /*DEBUGGER_H_*/
