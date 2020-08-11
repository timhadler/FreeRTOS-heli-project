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


motor_t mainMotor;
motor_t tailMotor;

uint32_t periodPWM;         // Period for the PWM


void initMotors(void) {
    SysCtlPWMClockSet(PWM_CLOCK_DIVIDER);

    // As a precaution, make sure that the peripherals used are reset
    SysCtlPeripheralReset (PWM_MAIN_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset (PWM_MAIN_PERIPH_PWM);  // Main Rotor PWM
    SysCtlPeripheralReset (PWM_TAIL_PERIPH_GPIO);
    SysCtlPeripheralReset (PWM_TAIL_PERIPH_PWM);  // Tail Rotor PWM

    //initialize all PWM, Main then tail
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    while (!SysCtlPeripheralReady(PWM_MAIN_PERIPH_PWM));
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);
    while (!SysCtlPeripheralReady(PWM_MAIN_PERIPH_GPIO));

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    while (!SysCtlPeripheralReady(PWM_TAIL_PERIPH_PWM));
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);
    while (!SysCtlPeripheralReady(PWM_TAIL_PERIPH_GPIO));

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


    // Calculate the PWM PeriodPWM corresponding to the freq.
    periodPWM = SysCtlClockGet() / PWM_DIVIDER / PWM_FREQ_HZ;
    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, periodPWM);
    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, periodPWM);

    // Setup motor structs
    mainMotor.duty = PWM_START_DUTY;
    mainMotor.freq = PWM_FREQ_HZ;
    mainMotor.base = PWM_MAIN_BASE;
    mainMotor.gen = PWM_MAIN_GEN;
    mainMotor.outnum = PWM_MAIN_OUTNUM;

    tailMotor.duty = PWM_START_DUTY;
    tailMotor.freq = PWM_FREQ_HZ;
    tailMotor.base = PWM_TAIL_BASE;
    tailMotor.gen = PWM_TAIL_GEN;
    tailMotor.outnum = PWM_TAIL_OUTNUM;
}


void setMotor(uint8_t main, uint8_t duty) {
    motor_t motor;

    if (main) {
        motor = mainMotor;
    } else {
        motor = tailMotor;
    }

    PWMPulseWidthSet(motor.base, motor.outnum, periodPWM * duty / 100);
}
