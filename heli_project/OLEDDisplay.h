/*
 * OLEDDisplay.h
 *
 *  Created on: 28/07/2020
 *      Author: tch118
 */

#ifndef OLEDDISPLAY_H_
#define OLEDDISPLAY_H_

//*****************************************************************************
// Global Variables
//*****************************************************************************
char TEXT_BUFFER[16];       // Text buffer for the display, each line can hold 16 characters

//*****************************************************************************
// Constants
//*****************************************************************************
enum displayLine {LINE_1 = 0, LINE_2, LINE_3, LINE_4};


//*****************************************************************************
// Functions
//*****************************************************************************

// Clears a line of the OLED display by writing a string of 16 spaces (" ") to it
void ClearDisplayLine(displayLine line);

// Writes a given string to the display on given line
void writeDisplay(char text[], displayLine line);


#endif /* OLEDDISPLAY_H_ */
