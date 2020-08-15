/*
 * OLEDDisplay.c
 *
 * Display source file for the Orbit OLED display used on the tm4c123gh6pm Tiva board
 * Developed for University of Canterbury, ENCE464 Heli project
 *
 *  Created on: 28/07/2020
 *      Author: Tim Hadler
 */

#include <stdint.h>
#include <stdio.h>
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OLEDDisplay.h"

//int16_t yaw = 6;
//extern int32_t altitude;
//extern int32_t meanVal;

// Initialise the Orbit OLED display
void initDisplay(void) {
    OLEDInitialise ();
}


// Clears a line of the OLED display by writing a string of 16 spaces (" ") to it
void ClearDisplayLine(uint8_t line) {
    OLEDStringDraw ("                ", 0, line);
}


// Writes a given string to the display on given line
// Clears the line being written to first
// Writes string starting at column = 0
void writeDisplay(char text[16], uint8_t line) {
    ClearDisplayLine(line);
    OLEDStringDraw(text, 0, line);
}

