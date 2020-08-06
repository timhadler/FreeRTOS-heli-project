/*
 * myMotors.h
 *
 *  Created on: 2/08/2020
 *      Author: tch118
 */

#ifndef MYMOTORS_H_
#define MYMOTORS_H_

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
// Constants
//*****************************************************************************
#define PWM_CLOCK_DIVIDER SYSCTL_PWMDIV_8
#define PWM_DIVIDER 8

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
#define PWM_MAIN_BASE        PWM0_BASE
#define PWM_MAIN_GEN         PWM_GEN_3
#define PWM_MAIN_OUTNUM      PWM_OUT_7
#define PWM_MAIN_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM  SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_5

#define PWM_TAIL_BASE        PWM1_BASE
#define PWM_TAIL_GEN         PWM_GEN_2
#define PWM_TAIL_OUTNUM      PWM_OUT_5
#define PWM_TAIL_OUTBIT      PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN    GPIO_PIN_1

#define PWM_START_FREQ 250
#define PWM_START_DUTY 50

#define MAIN 1
#define TAIL 0


//*****************************************************************************
// Funtion declarations
//*****************************************************************************
void initMotors(void);

void setMotor(bool main, uint32_t freq, uint8_t duty);




/*typedef struct Motor_S {
    uint32_t base;
    uint32_t gen;
    uint32_t outnum;
};*/

#endif /* MYMOTORS_H_ */
