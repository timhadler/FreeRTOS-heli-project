/*
 * controllers.h
 *
 *  Created on: 7/08/2020
 *      Author: tch118,
 *
 */

#ifndef CONTROLLERS_H_
#define CONTROLLERS_H_

#include <stdint.h>

void initControllers(void);

double getAltErr(void);

double getYawErr(void);

void piMainUpdate(void);

void piTailUpdate(void);




#endif /* CONTROLLERS_H_ */
