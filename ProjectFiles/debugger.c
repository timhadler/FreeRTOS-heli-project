
/*
 * debugger.c
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
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include "altitude.h"
#include "yaw.h"
#include "controllers.h"
#include "motors.h"
#include "debugger.h"


// initialiseUSB_UART - 8 bits, 1 stop bit, no parity
void initialiseUSB_UART (void)
{
    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);

    // Select the alternate (UART) function for these pins.
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}



//convert the landed state to a string for display
char* stateToString(uint8_t heliState)
{
    char* stateStr;

    if (heliState == LANDED) {
        stateStr = "Landed";

    } else if (heliState == LANDING) {
        stateStr = "Landing";

    } else if (heliState == TAKE_OFF) {
        stateStr = "Take off";

    } else if (heliState == IN_FLIGHT) {
        stateStr = "In flight";

    } else {
        stateStr = "Intd state";
    }
    return stateStr;
}


// Updates the UART communication
// Sends heli state, altitude, yaw, motor pwm duty cycles
//                  targetHeight, and targetYaw
void sendData(void* pvParameters)
{
    uint8_t heliState;
    char* stateStr;
    char statusStr[16 + 1];
    const uint16_t delay_ms = 1000/UART_SEND_RATE_HZ;

    while(1) {
        heliState = getState();
        stateStr = stateToString(heliState);

        // Form and send a status message to the console
        sprintf (statusStr, "Alt %d [%d] \r\n", getAlt(), getTargetAlt()); // * usprintf
        UARTSend (statusStr);
        sprintf (statusStr, "Yaw %d [%d] \r\n", getYaw(), getTargetYaw()); // * usprintf
        UARTSend (statusStr);
        sprintf (statusStr, "Main %d Tail %d \r\n", getTailPWM(), getMainPWM()); // * usprintf
        UARTSend (statusStr);
        sprintf(statusStr, "Mode: %s \r\n", stateStr);
        UARTSend (statusStr);

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

// Transmit a string via UART0
void UARTSend (char *pucBuffer)
{
    // Loop while there are more characters to send.
    while(*pucBuffer)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART_USB_BASE, *pucBuffer);
        pucBuffer++;
    }
}
