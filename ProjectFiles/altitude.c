/*  altitude.c - Reads the altitude using an ADC conversion and the average from a FreeRTOS Queue.
    Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Tim Hadler
    Last modified: 8.8.2020 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "altitude.h"

/* Constants */
#define QUEUE_SIZE 10 // Matches the number of samples per period of jitter, ensuring it will not significantly deviate
#define SAMPLE_RATE_HZ 100 // The sampling rate for altitude readings (well over the jitter of 4Hz)
#define VOLTAGE_SENSOR_RANGE 900 // The voltage range for the height sensor [mV]
#define QUEUE_ITEM_SIZE sizeof(uint32_t) //4 bytes which is the size of each ACD sample

/* Sets variables */
static int32_t altitude;
static int32_t meanVal;
static int32_t helicopter_landed_value;

/* FreeRTOS variables*/
static TimerHandle_t altitude_timer;
static QueueHandle_t ADCQueue;


/* The handler for the ADC conversion complete interrupt.
   Writes to the circular buffer */
void ADCIntHandler(void) {
    uint32_t sample;
    //
    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &sample);
    //
    // Place it in FreeRTOS Queue
    xQueueSendFromISR(ADCQueue, &sample, NULL);

    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

// Altitude Task function
void AltitudeTimerCallback(TimerHandle_t timer){
    ADCProcessorTrigger(ADC0_BASE, 3); // Initiate a conversion
}

/* Enables and configures ADC */
void initADC (void) {
    /* create a timer that would run the ADCProsessorTrigger */
    altitude_timer = xTimerCreate ("AltitudeTimer", pdMS_TO_TICKS(10), pdTRUE, NULL, AltitudeTimerCallback);
    /* create a timer that would run the ADCProsessorTrigger */
    ADCQueue = xQueueCreate(QUEUE_SIZE, QUEUE_ITEM_SIZE);

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


int32_t getMeanVal(void) {
    return meanVal;
}

int32_t getAlt(void) {
    return altitude;
}

/* Calculates the average mean  reading from the circular buffer and sets the landed value*/
void processAlt(void* pvParm) {
    int sum = 0;
    int i;
    int n = 0;
    int temp = 0;
        while(1){

            xTimerStart(altitude_timer, portMAX_DELAY); // start the timer (AltitudeTimer) and check if the Timer Queue is full with any time limit. So, it will keep checking the status of the queue for ever.
            xQueueReceive(ADCQueue, &temp, portMAX_DELAY);
            IntMasterDisable();
            sum = 0;
            for (i = 0; i < QUEUE_SIZE; i++) {
                sum = sum + temp;
                meanVal = (2 * sum + QUEUE_SIZE) / 2 / QUEUE_SIZE;
            }
            // Creates a delay so there are values in the buffer to use for the landed value
            if (n == QUEUE_SIZE) {
                helicopter_landed_value = meanVal;
                n++;
            } else if (n < QUEUE_SIZE) {
                n++;
            }
            altitude = ((100 * 2 * (helicopter_landed_value - meanVal) + VOLTAGE_SENSOR_RANGE)) / (2 * VOLTAGE_SENSOR_RANGE);
            IntMasterEnable();
        }
}
