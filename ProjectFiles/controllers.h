/*
 * controllers.h
 *
 * Contributers: Hassan Alhujhoj, Abdullah Naeem and Tim Hadler
 * Created on: 7/08/2020
 */

#ifndef CONTROLLERS_H_
#define CONTROLLERS_H_

#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "semphr.h"


//******************************************************************
// Macros
//******************************************************************
// Controller gains and constants
#define KP_M                    1
#define KI_M                    0.4
#define KP_T                    0.6
#define KI_T                    0.5
#define T_DELTA                 0.01
#define OUTPUT_MAX              95
#define OUTPUT_MIN              5
#define CONTROLLER_RATE_HZ      100
#define MAXIMUM_P_MAIN_CONTROL  20      // maximum proportional gain of main motor
#define MAXIMUM_I_MAIN_CONTROL  40      // maximum integral gain of main motor
#define MAXIMUM_P_TAIL_CONTROL  15      // maximum proportional gain of tail motor
#define MAXIMUM_I_TAIL_CONTROL  30      // maximum integral gain of tail motor

#define UPDATE_TARGET_RATE_HZ   3
#define NUM_SHAKES 4                    // Number of head shakes for special mode 2
#define WITHIN_5_DEGREES (yaw <= 5 || yaw >= 355)
#define UNASSIGNED 500                  // Yaw is limited to 0-360
#define CLAMP(Z, MIN, MAX)      (((Z) > (MAX)) ? (MAX) : (((Z) < (MIN)) ? (MIN) : (Z)))

//******************************************************************
// Global variables
//******************************************************************
SemaphoreHandle_t xTakeOffSemaphore;
SemaphoreHandle_t xButtPollSemaphore;
SemaphoreHandle_t xLandSemaphore;
SemaphoreHandle_t xFSMSemaphore;
enum heliStates {LANDED=0, LANDING, TAKE_OFF, IN_FLIGHT};


uint8_t getState(void);
uint8_t getTargetAlt(void);
int16_t getTargetYaw(void);
int16_t getAltErr(void);
int16_t getYawErr(void);

void initControllers(void);
void incAlt(void);
void decAlt(void);
void incYaw(void);
void decYaw(void);

void setMode1(void);
void setMode2(void);

void oneEighty(void);
void headShake(void);

void controller(void* pvParameters);
void piMainUpdate(void);
void piTailUpdate(void);

void FSM(void* pvParameters);
void takeOff(void* pvParameters);


#endif /* CONTROLLERS_H_ */
