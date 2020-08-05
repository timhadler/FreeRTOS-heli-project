/*
 * myTasks.h
 *
 *  Created on: 1/08/2020
 *      Author: Tim Hadler
 */

#ifndef MYTASKS_H_
#define MYTASKS_H_

//******************************************************************
// Constants
//******************************************************************

#define LED_BLINK_PERIOD_MS 2000
#define BUTTON_POLL_RATE_HZ 50
#define SAMPLE_RATE_HZ 250

//******************************************************************
// Functions
//******************************************************************

void blinkLED(void* pvParameters);

void pollButton(void* pvParameters);


#endif /* MYTASKS_H_ */
