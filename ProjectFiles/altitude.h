/*  altitude.c - Reads the altitude using an ADC conversion and the average from a FreeRTOS Queue.
    Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Tim Hadler
    Last modified: 8.8.2020 */

#ifndef ALTITUDE_H_
#define ALTITUDE_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"


/* The handler for the ADC conversion complete interrupt.
   Writes to the circular buffer */
void
ADCIntHandler(void);

void
initADC (void);

uint32_t
getAlt(void);

/* A FreeRTOS task that calculates the mean altitude readings from the FreeRTOS queue and measures the altitude based on the mean ADC value. */
void
processAlt(void* pvParameters);

void
AltitudeTimerCallback(TimerHandle_t timer);

#endif /* ALTITUDE_H_ */
