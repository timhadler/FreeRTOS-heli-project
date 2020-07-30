/*
 * buffer.h
 *
 *
 *  Created on: 30/07/2020
 *      Author: tch118
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>

#define BUF_SIZE 10     // Circular buffer size
#define SAMPLE_RATE_HZ 250

void initBuffer(void);

void writeBuffer(uint32_t value);

uint16_t getBufferAvg(void);


#endif /* BUFFER_H_ */
