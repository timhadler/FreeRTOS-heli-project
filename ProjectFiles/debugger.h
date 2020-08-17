#ifndef DEBUGGER_H_
#define DEBUGGER_H_


#include <stdbool.h>



#define QUEUE_LENGTH            200
#define BAUD_RATE               9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX




int initdebugger(void);

void debug_log_task(void *d);

bool dprintf (const char *format, ...);

bool dputs(const char *string);


#endif /* DEBUGGER_H_ */
