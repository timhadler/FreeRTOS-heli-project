#include <stdint.h>
#include <stdbool.h>


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"

#include "FreeRTOS.h"
#include "myYaw.h"
#include "altitude.h"
#include "controllers.h"
#include "control_command.h"
#include "debugger.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"


#define CONTROLLER_RATE_HZ 250
#define UNDEFINED_YAW 400
#define MAX_YAW 360

bool mode1 = false;
bool mode2 = false;
static int32_t targetYawRef = UNDEFINED_YAW;
static int32_t current_yaw = UNDEFINED_YAW;
int count = 0;


/* Increases the set altitude by 10% */
void
incrAlt(void)
{
    // Limits the set altitude to <=100% and can only be changed while landed or flying
    if (targetAlt < 100) {
        targetAlt += 10;
    }
    dprintf ("Alt %d \n", targetAlt);
}

/* Decreases the set altitude by 10% */
void
decrAlt(void)
{
    // Limits the set altitude to >=0% and can only be changed while landed or flying
    if (targetAlt > 0) {
        targetAlt -= 10;
    }
    dprintf ("Set Alt %d \n", targetAlt);
}

/* Increases the set yaw by 15 degrees */
void
incrYaw(void)
{
    if (targetYaw == 345) {
        targetYaw = 0;
    } else {
        targetYaw += 15;
    }
    dprintf ("Set Yaw %d \n", targetYaw);
}

/* Decreases the set yaw by 15 degrees */
void
decrYaw(void)
{
    if (targetYaw == 0) {
           targetYaw = 345;
    } else {
           targetYaw -= 15;
    }
    dprintf ("Set Yaw %d \n", targetYaw);
}

int32_t
getCurrentYaw(void)
{
    targetYawRef = getYaw();
    if (targetYawRef >= MAX_YAW) {
        targetYawRef = targetYawRef - MAX_YAW;
    }
    return targetYawRef;

}

void no_nod(void)
{
    static bool clockwise = 1;
    static int32_t m2targetYaw;
    static int n_nods = 0;

    if (clockwise) {
        if (targetYawRef == UNDEFINED_YAW) {
            targetYawRef = getCurrentYaw() + 60;
        }
    } else {
        if (targetYawRef == UNDEFINED_YAW) {
            targetYawRef = getCurrentYaw() - 60;
        }
    }
    if (count == 500) {
        current_yaw = getYaw();
        m2targetYaw = current_yaw + 10;
        //dprintf ("m2y %d \n", m2targetYaw);
        dprintf("target30 %d \n", targetYawRef);
        dprintf ("st %d \n", clockwise);
        count = 0;
    } else if ((current_yaw > (targetYawRef - 5)  && current_yaw < (targetYawRef + 5)) == 1) {
        clockwise = !clockwise;
        n_nods += 1;
        targetYawRef = UNDEFINED_YAW;
    } if (n_nods  == 100) {
        mode2 = false;
        n_nods = 0;
    }
    //dprintf ("current Yaw %d \n", current_yaw);
    piMainUpdate(targetAlt);
    piTailUpdate(m2targetYaw);
}



void oneeighty_midalt(void)
{
    static int32_t m1targetYaw;
    static int32_t m1targetAlt;

    if (targetYawRef == UNDEFINED_YAW) {
        targetYawRef = getYaw() + 180;
        if (targetYawRef > 360) {
            targetYawRef = targetYawRef - 360;
        }
    }
    current_yaw = getYaw();

    if (count == 50) {
        current_yaw = getYaw();
        //m1targetAlt = 50;
        m1targetYaw = current_yaw + 5;
        dprintf ("m1 target Yaw %d \n", m1targetYaw);
        dprintf("target180 %d \n", targetYawRef);
        count = 0;
    } else if ((current_yaw > (targetYawRef - 10)  && current_yaw < (targetYawRef + 10)) == 1) {
        current_yaw = UNDEFINED_YAW;
        targetYawRef = UNDEFINED_YAW;
        mode1 = false;
    }
    piMainUpdate(targetAlt);
    piTailUpdate(m1targetYaw);
}


void controller_command(void* pvParameters) {
    xControlSemaphore = xSemaphoreCreateBinary();
    const uint16_t delay_ms = 1000/CONTROLLER_RATE_HZ;

    xSemaphoreTake(xControlSemaphore, portMAX_DELAY);
    targetYaw = getReference();
    while(1) {

        if (count > 1000 && mode1 == false) {
            mode1 = true;
            count = 0;
        }

        if (mode1 == true) {
            oneeighty_midalt();
        }
        if (mode2 == true) {
            no_nod();
        }
        else {
            piMainUpdate(targetAlt);
            piTailUpdate(targetYaw);
        }

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        count++;
    }
}












