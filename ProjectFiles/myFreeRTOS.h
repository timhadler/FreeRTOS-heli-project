/*
 * myFreeRTOS.h
 *
 *  Created on: 1/08/2020
 *      Author: Tim Hadler
 */


/* Note on FreeRTOS: there must always be a function that is ready to execute.
//                   For this reason freeRTOS automatically creates a task called Idle, that does pretty much nothing,
//                   Idle has lowest prioirty (0) so it does not preempt higher prioritized tasks
//                   However time-slicing may occur with other tasks assigned a priority of 0
//                   Can force idle to always yield by configering in freeRTOS_config.h

                     Interrupts have priority over tasks, ie the lowest priority interrupt will
                     pre-empt the highest priority task
                     Note: Never call a FreeRTOS API function that does not have “FromISR” in its name from an ISR

                     use atleast one semaphore, ine queue, and one mutex in the project
ISR.
*/


#ifndef MYFREERTOS_H_
#define MYFREERTOS_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"


// creates a task with FreeRTOS
// function - function handle for task
// taskName - name used only for debugging purposes,
//            can create multiple tasks from the same function by assigning a different taskName
// stackDepth - number of wordlengths of stack to give to the task
// priority - 0-MAX_PRIORITIES-1, defines in freeRTOS_config.h
// task handle - this can be used to create a global task handle for the task,
//               the handle can be used by other tasks to do things like change priority, or delete/suspend task
void createTask(TaskFunction_t function, const char* const taskName, const configSTACK_DEPTH_TYPE stackDepth,
                void* const parameters, UBaseType_t priority, TaskHandle_t* const taskHandle);

// Initiates FreeRTOS
void startFreeRTOS(void);

// Creates a task delay through FreeRTOS for a given time in milliseconds
void taskDelayMS(uint16_t delay_ms);

void createSemaphores();

#endif /* MYFREERTOS_H_ */
