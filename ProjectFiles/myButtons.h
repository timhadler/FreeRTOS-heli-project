/*
 * myButtons.h
 *
 *  Created on: 11/08/2020
 *      Author: tch118
 */

#ifndef MYBUTTONS_H_
#define MYBUTTONS_H_


#define BUTTON_POLL_RATE_HZ 80


void pollButton(void* pvParameters);

uint8_t getTargetAlt(void);

int16_t getTargetYaw(void);


#endif /* MYBUTTONS_H_ */
