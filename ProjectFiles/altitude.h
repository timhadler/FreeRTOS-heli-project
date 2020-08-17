/*  altitude.c - Reads the altitude using an ADC conversion and the average from a FreeRTOS Queue.
    Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Tim Hadler
    Last modified: 8.8.2020 */

#ifndef ALTITUDE_H_
#define ALTITUDE_H_

#include <stdint.h>
#include "timers.h"

//******************************************************************
// Constants / Macros
//******************************************************************
#define Alt_QUEUE_SIZE       10                  // The size of the queue containing the raw ADC values.
#define Alt_OUT_QUEUE_SIZE      1                   // The size of the queue containg the final altitude value.
#define SAMPLE_RATE_HZ 100                          // The sampling rate for altitude readings (well over the jitter of 4Hz)
#define VOLTAGE_SENSOR_RANGE    900                 // The voltage range for the height sensor [mV]
#define QUEUE_ITEM_SIZE         sizeof(uint32_t)    //4 bytes which is the size of each ACD sample
#define minAlt                  2300                // The minimum voltage range for the height sensor [mV]
#define maxAlt                  1100                // The maximum voltage range for the height sensor [mV]


/* The handler for the ADC conversion complete interrupt.
   Writes to the circular buffer */
void
ADCIntHandler(void);

void
initADC (void);

uint32_t
getAlt(void);

uint32_t
getMidAlt(void);

/* A FreeRTOS task that calculates the mean altitude readings from the FreeRTOS queue and measures the altitude based on the mean ADC value. */
void
processAlt(void* pvParameters);

void
AltitudeTimerCallback(TimerHandle_t timer);

#endif /* ALTITUDE_H_ */
