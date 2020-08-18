#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>



#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"


#include "debugger.h"
#include "FreeRTOS.h"
#include "queue.h"


static  QueueHandle_t _debug_queue;




//********************************************************
// initialiseUSB_UART - 8 bits, 1 stop bit, no parity
//********************************************************

int initdebugger(void)
{
    // Enable GPIO port A which is used for UART0 pins.
    //
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);

    //Create a queue big enough to hold a few lines of buffered text
    //If this is too short, we will frequently overflow and truncate
    //our output. If it is too long we are jsut wasting memory.
    _debug_queue = xQueueCreate(QUEUE_LENGTH, 1);
    if (! _debug_queue)
        return -1;

    // Spawn the queue-monitoring task

    return 0;
}

void debug_log_task(void *d)
{
    while(1) {
        char ch;
        if (xQueueReceive(_debug_queue, &ch, portMAX_DELAY))
            UARTCharPut(UART_USB_BASE, ch);
    }
}


//ellipses to indicate that it can accept a variable number of arguments.
bool dprintf (const char *format, ...)
{
    char buffer[128];
    va_list ap; //Variable argument list stores a variable number of arguments


    if (!_debug_queue)
        return false;

    va_start(ap, format);
    vsnprintf(buffer, sizeof(buffer), format, ap); //same as printf but used when getting element from argument list.
    va_end(ap);

    return dputs(buffer);
}

bool dputs(const char *string) {



    while(*string) {
        if (!xQueueSend(_debug_queue, string++, 0))
            return false;
    }
    return true;

}







