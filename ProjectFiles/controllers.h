/*
 * controllers.h
 *
 *  Created on: 7/08/2020
 *      Author: tch118
 */

#ifndef CONTROLLERS_H_
#define CONTROLLERS_H_

#include <stdint.h>

void initControllers(void);

int32_t getAltErr(int32_t cMean);

int32_t getYawErr(int16_t cYaw);

void updateControl(int32_t altError, int32_t yawError);



#endif /* CONTROLLERS_H_ */
