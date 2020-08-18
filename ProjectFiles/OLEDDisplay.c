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
#include "control_command.h"
#include "altitude.h"
#include "myYaw.h"

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

void displayOLED(void* pvParameters) {
    const uint16_t delay_ms = 1000/DISPLAY_RATE_HZ;

    char text_buffer[16];
    while(1) {
        // Display Height
        sprintf(text_buffer, "Altitude: %d%%", getAlt());
        writeDisplay(text_buffer, LINE_1);

        // Display yaw
        sprintf(text_buffer, "Yaw: %d", getYaw());
        writeDisplay(text_buffer, LINE_2);

        sprintf(text_buffer, "Target Alt: %d%%", targetAlt);
        writeDisplay(text_buffer, LINE_3);

        sprintf(text_buffer, "Target Yaw: %d", targetYaw);
        writeDisplay(text_buffer, LINE_4);


        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}


