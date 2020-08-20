/*
 *
 * altitude.h
 *
 *  Reads the altitude using an ADC conversion and the average from a FreeRTOS Queue.
 *
 *  Contributers: Hassan Alhujhoj, Abdullah Naeem and Tim Hadler
 *  Last modified: 8.8.2020
 *
 */

#ifndef ALTITUDE_H_
#define ALTITUDE_H_

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

//******************************************************************
// Macros - Constants
//******************************************************************
#define Alt_QUEUE_SIZE              10
#define SAMPLE_RATE_HZ              100
#define QUEUE_ITEM_SIZE             sizeof(uint32_t)
#define minAlt                      2300
#define maxAlt                      1100
#define Alt_IN_QUEUE_SIZE           10
#define SAMPLE_RATE_HZ              100
#define VOLTAGE_SENSOR_RANGE        962


/**
 * The handler for the ADC conversion complete interrupt.
 * Writes to the circular buffer
 */
void ADCIntHandler(void);

void initADC (void);

uint8_t getAlt(void);

uint32_t getMidAlt(void);

/**
 * A FreeRTOS task that calculates the mean altitude readings from the
 * FreeRTOS queue and measures the altitude based on the mean ADC value.
 */
void processAlt(void* pvParameters);

void AltitudeTimerCallback(TimerHandle_t timer);

#endif /* ALTITUDE_H_ */
