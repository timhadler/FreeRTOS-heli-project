/*
 * user_input.h
 *
 *  Created on: 18/08/2020
 *      Author: tch118
 */

#ifndef USER_INPUT_H_
#define USER_INPUT_H_

#include <stdint.h>


// Define MODE switch
#define MODE_PERIPH_GPIO SYSCTL_PERIPH_GPIOA
#define SWITCH_MODE_GPIO_BASE GPIO_PORTA_BASE
#define SWITCH_MODE_INT_PIN GPIO_INT_PIN_7
#define SWITCH_MODE_PIN GPIO_PIN_7

#define BUTTON_POLL_RATE_HZ 60


void initModeSwitch(void);

void SwitchModeIntHandler(void);

void pollButton(void* pvParameters);



#endif /* USER_INPUT_H_ */
