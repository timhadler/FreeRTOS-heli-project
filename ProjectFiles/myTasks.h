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


//******************************************************************
// Global Variables
//******************************************************************



//******************************************************************
// Functions
//******************************************************************

void blinkLED(void* pvParameters);

void pollButton(void* pvParameters);

void processYaw(void* pvParameters);

void displayOLED(void* pvParameters);


#endif /* MYTASKS_H_ */
