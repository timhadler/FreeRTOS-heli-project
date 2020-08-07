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
#include "myFreeRTOS.h"

//int16_t yaw = 6;


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


void displayOLED(void* pvParameters) {
    char text_buffer[16];
    int16_t yaw = 0;

    while(1) {
        yaw = *(int16_t*) pvParameters;
        sprintf(text_buffer, "Yaw: %d", yaw);
        writeDisplay(text_buffer, 1);
        taskDelayMS(1000/DISPLAY_RATE_HZ);
    }
}
