/*
 * altitude.c
 *
 * Reads the altitude using an ADC conversion algorithm and averages these ADC
 * readings from a FreeRTOS Queue to get the helicopter altitude in percentage.
 *
 * Contributers: Hassan Alhujhoj, Abdullah Naeem and Tim Hadler
 * Last modified: 08/08/2020
 */


#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"
#include "altitude.h"

//******************************************************************
// Global Variables
//******************************************************************
static int16_t altitude = 0;
static TimerHandle_t Alt_IN_Timer;
static QueueHandle_t Alt_IN_Queue;


// Returns current altitude
uint8_t getAlt(void)
{
    return altitude;
}


// The handler for the ADC conversion complete interrupt.
void ADCIntHandler(void)
{
    uint32_t sample;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    //Get the single sample from ADC0.  ADC_BASE is defined in inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &sample);

    // Place it in FreeRTOS Queue
    xQueueSendFromISR(Alt_IN_Queue, &sample, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    ADCIntClear(ADC0_BASE, 3);
}


// Altitude Task function. A callback function for the created timer in initADC().
void AltitudeTimerCallback(TimerHandle_t timer)
{
    ADCProcessorTrigger(ADC0_BASE, 3);      // Initiate an ADC conversion
}


// Enables and configures ADC
void initADC (void)
{
    // Create a timer that would run the ADCProsessorTrigger
    Alt_IN_Timer = xTimerCreate ("AltitudeTimer", pdMS_TO_TICKS(10), pdTRUE, NULL, AltitudeTimerCallback);

    if(Alt_IN_Timer == NULL)
    {
        while(1);
    }

    // Create a FreeRTOS queue for average mean of ADC readings
    Alt_IN_Queue = xQueueCreate(Alt_QUEUE_SIZE, QUEUE_ITEM_SIZE);
    if(Alt_IN_Queue == NULL)
    {
        while(1);
    }

    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    // Set to pin PE4
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);

    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}


/**
 * Calculates the average mean of ADC readings and altitude of the helicopter from a
 *  FreeRTOS queue
 */
void processAlt(void* pvParameter)
{
    uint32_t temp = 0;
    uint32_t sum = 0;
    uint8_t count = 0;
    uint32_t avg = 0;
    uint32_t landedAlt = 0;

    // Start the timer that initiates ADC conversions
    xTimerStart(Alt_IN_Timer, portMAX_DELAY);

    while(1)
    {
        // Receives ADC sample from the queue, blocks task if queue is empty
        xQueueReceive(Alt_IN_Queue, &temp, portMAX_DELAY);
        sum += temp;
        count++;

        // Averages the ADS samples
        if (count == Alt_IN_QUEUE_SIZE)
        {
            avg = (2 * sum + Alt_IN_QUEUE_SIZE) / 2 / Alt_IN_QUEUE_SIZE;
            sum = 0;
            count = 0;

            // First time the buffer is filled, heli should be landed, record the averaged adc values
            if (landedAlt == 0)
            {
                landedAlt = avg;
            }
        }

        altitude = 100 * (landedAlt - avg) / VOLTAGE_SENSOR_RANGE;

        // Limit alt to 0-100%
        if (altitude > 100)
        {
            altitude = 100;
        } else if (altitude < 0)
        {
            altitude = 0;
        }
    }
}
