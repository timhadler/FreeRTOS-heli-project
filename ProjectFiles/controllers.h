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
// Macros - Define ref signal base and pins
//******************************************************************
#define REF_PERIPH              SYSCTL_PERIPH_GPIOC
#define REF_GPIO_BASE           GPIO_PORTC_BASE
#define REF_INT_PIN             GPIO_INT_PIN_4
#define REF_PIN                 GPIO_PIN_4
#define KP_M                    1
#define KI_M                    0.4
#define KP_T                    0.6
#define KI_T                    0.5
#define T_DELTA                 0.01
#define OUTPUT_MAX              95
#define OUTPUT_MIN              5
#define CONTROLLER_RATE_HZ      100
#define UPDATE_TARGET_RATE_HZ   3
#define BUTTON_POLL_RATE_HZ     60
#define MAXIMUM_P_MAIN_CONTROL  20      // maximum proportional gain of main motor
#define MAXIMUM_I_MAIN_CONTROL  40      // maximum integral gain of main motor
#define MAXIMUM_P_TAIL_CONTROL  15      // maximum proportional gain of tail motor
#define MAXIMUM_I_TAIL_CONTROL  30      // maximum integral gain of tail motor
#define CLAMP(Z, MIN, MAX)      (((Z) > (MAX)) ? (MAX) : (((Z) < (MIN)) ? (MIN) : (Z)))

//******************************************************************
// Globals to module
//******************************************************************
static int16_t refYaw;
static uint8_t state;
static uint8_t targetAlt;
static int16_t targetYaw;
static bool foundRef;
static bool mode1_flag;
static bool mode2_flag;
SemaphoreHandle_t xTakeOffSemaphore;
SemaphoreHandle_t xButtPollSemaphore;
SemaphoreHandle_t xLandSemaphore;
SemaphoreHandle_t xFSMSemaphore;
enum heliStates {LANDED=0, LANDING, TAKE_OFF, IN_FLIGHT};

int16_t getRefYaw(void);

uint8_t getState(void);

uint8_t getTargetAlt(void);

int16_t getTargetYaw(void);

int16_t getAltErr(int16_t setAlt);

int16_t getYawErr(int16_t setAlt);

void initControllers(void);

void incAlt(void);

void decAlt(void);

void incYaw(void);

void decYaw(void);

void setMode1(void);

void setMode2(void);

void piMainUpdate(void);

void piTailUpdate(void);

void FSM(void* pvParameters);

void takeOff(void* pvParameters);

void land(void* pvParameters);

void controller(void* pvParameters);

#endif /* CONTROLLERS_H_ */
