/*
 * buffer.c
 *
 *
 *  Created on: 30/07/2020
 *      Author: tch118
 */

#include <stdint.h>
#include "circBufT.h"
#include "buffer.h"

//******************************************************************
// Global Variables
//******************************************************************

static circBuf_t g_inBuffer;
//static uint32_t g_ulSampCnt;      Dont think we need this, only appeared as g_ulSampCnt++; in SysTickIntHandler


//******************************************************************
// Functions
//******************************************************************
void initBuffer(void) {
    initCircBuf (&g_inBuffer, BUF_SIZE);
}


void writeBuffer(uint32_t value) {
    writeCircBuf (&g_inBuffer, value);
}


// Function to calculate the average of the values in buffer
uint16_t getBufferAvg(void) {
    uint16_t sum = 0;
    uint16_t average = 0;
    uint8_t i;

    for (i = 0; i < BUF_SIZE; i++) {
        sum += readCircBuf (&g_inBuffer);
    }
    average = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;
    //avg = sum/BUF_SIZE;
    return average;
}
