/*
 * myMotors.c
 *
 *  Created on: 2/08/2020
 *      Author: tch118
 */

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "myMotors.h"


void initMotors(void) {
    SysCtlPWMClockSet(PWM_CLOCK_DIVIDER);

    // As a precaution, make sure that the peripherals used are reset
    SysCtlPeripheralReset (PWM_MAIN_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset (PWM_MAIN_PERIPH_PWM);  // Main Rotor PWM
    SysCtlPeripheralReset (PWM_TAIL_PERIPH_GPIO);
    SysCtlPeripheralReset (PWM_TAIL_PERIPH_PWM);  // Tail Rotor PWM

    //initialize all PWM, Main then tail
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);

    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);

    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    //PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);

    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    //PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);

    // Initialisation is complete, so turn on the output.
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}


void setMotor(bool main, uint32_t freq, uint8_t duty) {
    uint32_t base = 0;
    uint32_t gen = 0;
    uint32_t outnum = 0;
    uint32_t period = 0;

    if (main) {
        base = PWM_MAIN_BASE;
        gen = PWM_MAIN_GEN;
        outnum = PWM_MAIN_OUTNUM;
    } else {
        base = PWM_TAIL_BASE;
        gen = PWM_TAIL_GEN;
        outnum = PWM_TAIL_OUTNUM;
    }

    // Calculate the PWM period corresponding to the freq.
    period = SysCtlClockGet() / PWM_DIVIDER / freq;

    PWMGenPeriodSet(base, gen, period);
    PWMPulseWidthSet(base, outnum, period * duty / 100);
}



/*class Motor_S
{
    char[5] _name;
    int _freq;
    int _duty;

public:
    Motor_S(char[5] name) {
        _name = name;
    }
};


void Motor_S::Init() {
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);

    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    // Set the initial PWM parameters
    setPWM (PWM_START_RATE_HZ, PWM_START_DUTY);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    //PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
}

void Motor_S::SetMotor(uint32_t freq, uint8_t duty) {
    // Calculate the PWM period corresponding to the freq.
    uint32_t period = SysCtlClockGet() / PWM_DIVIDER / freq;

    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
        period * duty / 100);
}*/
