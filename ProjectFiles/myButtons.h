/*
 * myButtons.h
 *
 *  Created on: 11/08/2020
 *      Author: tch118
 */

#ifndef MYBUTTONS_H_
#define MYBUTTONS_H_

// Define MODE switch
#define MODE_PERIPH_GPIO SYSCTL_PERIPH_GPIOA
#define SWITCH_MODE_GPIO_BASE GPIO_PORTA_BASE
#define SWITCH_MODE_INT_PIN GPIO_INT_PIN_7
#define SWITCH_MODE_PIN GPIO_PIN_7


#define BUTTON_POLL_RATE_HZ 80

void SwitchModeIntHandler(void);

void initModeSwitch(void);



#endif /* MYBUTTONS_H_ */
