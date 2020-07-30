/*
 * Main source file for ENCE464 Heli project
 *
 *  Created on: 27/07/2020
 *      Authors: tch118, ...
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"

#include "driverlib/adc.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "utils/ustdlib.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "FreeRTOS.h"
#include "task.h"
#include "buttons4.h"

#include "OLEDDisplay.h"
#include "constants.h"
#include "buffer.h"


//******************************************************************
// Global Variables
//******************************************************************
// global for now
char text_buffer[16];
uint16_t height = 0;


//******************************************************************
// Functions
//******************************************************************
void ADCIntHandler(void) {
    uint32_t sample;

    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &sample);
    //
    // Place it in the circular buffer (advancing write index)
    writeBuffer(sample);
    //
    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}


void SysTickIntHandler(void) {
    ADCProcessorTrigger(ADC0_BASE, 3);  // Initiate ADC conversion
    //g_ulSampCnt++;
}


//function to calculate current height as percentage
//TODO: Figure out how to to calculate height as a percentage
//      The value returned by getBuffAvg is fine, just need to convert it to height
uint16_t getHeight(void) {

    int8_t height = 0;

    //static uint16_t landedAvg = 0;
    uint16_t buffAvg = 0;

/*    if (landedAvg == 0) {
        landedAvg = getBufferAvg();
        return landedAvg;
    }*/

    buffAvg = getBufferAvg();
    height = MAX_HEIGHT/buffAvg * 1000;
    //diff = (landedAvg - buffAvg)*100;// * 100;
    //height = (int8_t) (diff / MAX_HEIGHT);

    return height;
}


void blinkLED(void *pvParameters) {
    uint8_t pin = (*(uint8_t *) pvParameters);
    uint8_t current = 0;

    while(1) {
        current ^= pin;
        GPIOPinWrite(LED_GPIO_BASE, pin, current);
        vTaskDelay(LED_BLINK_RATE / portTICK_RATE_MS);
    }
}


void initADC(void) {
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    // TODO: This delay allows the peripheral to fully configure, adjust value 1 second is too long
    SysCtlDelay(SysCtlClockGet() / 3);// 1 sec delay

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
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);

    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}


void initClocks(void) {
    // Set the clock rate to 80 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);

    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}


// Initialize the program
void initialize(void) {
    initClocks();
    initADC();
    initButtons();
    initDisplay();
    initBuffer();
    static uint8_t led = LED_RED_PIN;

    // For LED blinky task - initialize GPIO port F and then pin #1 (red) for output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);                // activate internal bus clocking for GPIO port F
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));        // busy-wait until GPIOF's bus clock is ready

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);         // PF_1 as output
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);    // doesn't need too much drive strength as the RGB LEDs on the TM4C123 launchpad are switched via N-type transistors
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);               // off by default



    if (pdTRUE != xTaskCreate(blinkLED, "Happy LED go blink blink", 32, (void *) &led, 4, NULL))
    {
        while(1);               // Oh no! Must not have had enough memory to create the task.
    }
}


void main(void) {
    initialize();
    IntMasterEnable();
    //vTaskStartScheduler();

    //*******************
    // Code to test buttons
    //*******************
    uint8_t count = 0;
    uint32_t clock_rate = SysCtlClockGet();

    while(1) {
        updateButtons();
        if (checkButton (UP) == PUSHED) {
            count++;
            sprintf(text_buffer, "Button Presses %d", count);
            writeDisplay(text_buffer, LINE_1);
            //OLEDStringDraw(text_buffer, 0, 1);
        }


        height = getHeight();
        //sprintf(text_buffer, "                ", height);
        sprintf(text_buffer, "Height: %d", height);
        writeDisplay(text_buffer, LINE_2);


        SysCtlDelay(clock_rate / 150);
    }
    //*********************
}
