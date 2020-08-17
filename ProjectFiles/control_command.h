#ifndef CONTROL_COMMAND_H_
#define CONTROL_COMMAND_H_



#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "FreeRTOS.h"
#include "semphr.h"


uint8_t targetAlt;
int16_t targetYaw;


SemaphoreHandle_t xControlSemaphore;


void
incrAlt(void);

void
decrAlt(void);

void
incrYaw(void);

void
decrYaw(void);

void
controller_command(void* pvParameters);

#endif /*CONTROL_COMMAND_H_ */
