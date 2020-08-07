/*  altitude.h - Reads the altitude using an ADC conversion and the average of a circular buffer. */

/*  Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
    Last modified: 1.6.2019
    Based on ADCdemo1.c by P.J. Bones UCECE */

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

/* Enables and configures ADC */
void
initADC (void);

int32_t getAlt(void);

/* A FreeRTOS task that calculates the mean altitude readings from the FreeRTOS queue and measures the altitude based on the mean ADC value. */
void
xProcessAltData(void* pvParameters);

void AltitudeTimerCallback(TimerHandle_t timer);

#endif /* ALTITUDE_H_ */
