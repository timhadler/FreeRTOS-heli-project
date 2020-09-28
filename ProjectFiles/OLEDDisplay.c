/*
 *  OLEDDisplay.c
 *
 *  Display source file for the Orbit OLED display used on the tm4c123gh6pm Tiva board
 *  Developed for University of Canterbury, ENCE464 Heli project
 *
 *  Contributers: Hassan Alhujhoj, Abdullah Naeem and Tim Hadler
 *  Created on: 28/07/2020
 */

#include <stdint.h>
#include <stdio.h>
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OLEDDisplay.h"
#include "altitude.h"
#include "yaw.h"
#include "controllers.h"

// Initialise the Orbit OLED display
void initDisplay(void)
{
    OLEDInitialise ();
}

// Clears a line of the OLED display by writing a string of 16 spaces (" ") to it
void ClearDisplayLine(uint8_t line)
{
    OLEDStringDraw ("                ", 0, line);
}

/**
 * Writes a given string to the display on given line
 * Clears the line being written to first
 * Writes string starting at column = 0
 */
void writeDisplay(char text[16], uint8_t line)
{
    ClearDisplayLine(line);
    OLEDStringDraw(text, 0, line);
}

//Displays the current and target values for the altitude and the yaw
void displayOLED(void* pvParameters)
{
    const uint16_t delay_ms = 1000/DISPLAY_RATE_HZ;

    char text_buffer[16];
    while(1) {
        // Display Height
        sprintf(text_buffer, "Altitude: %d%%", getAlt());
        writeDisplay(text_buffer, LINE_1);
        // Display yaw
        sprintf(text_buffer, "Yaw: %d", getYaw());
        writeDisplay(text_buffer, LINE_2);
        // Target height
        sprintf(text_buffer, "Target Alt: %d%%", getTargetAlt());
        writeDisplay(text_buffer, LINE_3);
        // Target yaw
        sprintf(text_buffer, "Target Yaw: %d", getTargetYaw());
        writeDisplay(text_buffer, LINE_4);

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}



