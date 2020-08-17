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


#include "controllers.h"
#include "control_command.h"
#include "debugger.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"


#define CONTROLLER_RATE_HZ 250





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



void controller_command(void* pvParameters) {
    xControlSemaphore = xSemaphoreCreateBinary();
    const uint16_t delay_ms = 1000/CONTROLLER_RATE_HZ;

    xSemaphoreTake(xControlSemaphore, portMAX_DELAY);
    targetYaw = getReference();
    while(1) {

        piMainUpdate(targetAlt);
        piTailUpdate(targetYaw);

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}












