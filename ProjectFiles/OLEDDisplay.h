/*
 * OLEDDisplay.h
 *
 * Display header file for the Orbit OLED display used on the tm4c123gh6pm Tiva board
 * Developed for University of Canterbury, ENCE464 Heli project
 *
 *  Created on: 28/07/2020
 *      Author: Tim Hadler
 */

#ifndef OLEDDISPLAY_H_
#define OLEDDISPLAY_H_

#include <stdint.h>


//*****************************************************************************
// Constants
//*****************************************************************************
#define DISPLAY_RATE_HZ 15

enum displayLine {LINE_1 = 0, LINE_2, LINE_3, LINE_4};


//*****************************************************************************
// Functions
//*****************************************************************************

// Initialise the Orbit OLED display
void initDisplay(void);

// Clears a line of the OLED display by writing a string of 16 spaces (" ") to it
void ClearDisplayLine(uint8_t line);

// Writes a given string to the display on given line
// Clears the line being written to first
// Writes string starting at column = 0
void writeDisplay(char text[16], uint8_t line);

void displayOLED(void* pvParameters);

#endif /* OLEDDISPLAY_H_ */
