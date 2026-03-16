/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"
#include <inttypes.h>




/*-----------------------------------------------------------*/
#define mainQUEUE_LENGTH 100

void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName );
void vApplicationMallocFailedHook( void );
typedef enum task_type {PERIODIC, APERIODIC} task_type;

typedef struct dd_task
{
	TaskHandle_t t_handle;
	task_type type;
	uint32_t task_id;
	uint32_t release_time;
	uint32_t absolute_deadline;
	uint32_t completion_time;
} dd_task;

//(TASK LIST) called node instead of list
typedef struct dd_task_node
{
	dd_task *task;
	struct dd_task_node *next;
} dd_task_node;

dd_task_node *active_list = NULL;
dd_task_node *completed_list = NULL;
dd_task_node *overdue_list = NULL;

typedef enum
{
	WORKLOAD_1,
	WORKLOAD_2,
	WORKLOAD_3,

} workload_type;

//copy of dd_task_list
#define MAX_TASKS 10
typedef struct
{
	int count;
	dd_task tasks[MAX_TASKS];
} dd_task_list;

typedef enum
{
	GET_OVERDUE_LIST,
	GET_ACTIVE_LIST,
	GET_COMPLETED_LIST
} dd_task_list_req;

void release_dd_task(TaskHandle_t t_handle,
					task_type type,
					uint32_t task_id,
					uint32_t absolute_deadline);

void complete_dd_task(uint32_t task_id);

void xMonitor_Task(void *pvParameters);

uint32_t get_active_task_list(void);
uint32_t get_overdue_dd_task_list(void);
uint32_t get_completed_dd_task_list(void);

void check_overdue_tasks(uint32_t currentTime);
void assign_priorities();
void move_to_completed(uint32_t the_task_id);
void insert_sorted(dd_task *new_task);

void xWorkloadTask_1(void *pvParameters);
void xWorkloadTask_2(void *pvParameters);
void xWorkloadTask_3(void *pvParameters);

void xDeadline_Driven_Task_Generator();
void xDDS_Task();

QueueHandle_t xRespQueue = NULL;
QueueHandle_t xReqQueue = NULL;
QueueHandle_t xCompleteQueue = NULL;
QueueHandle_t xReleaseQueue = NULL;
QueueHandle_t xCreateQueue = NULL;

#define MAX_USER_TASK_PRIORITY (configMAX_PRIORITIES - 1)





/*-----------------------------------------------------------*/

int main(void)
{



	xRespQueue = xQueueCreate(5, sizeof(uint32_t));
	xReqQueue = xQueueCreate(5, sizeof(dd_task_list_req));
	xReleaseQueue = xQueueCreate(10, sizeof(dd_task));
	xCompleteQueue = xQueueCreate(10, sizeof(uint32_t));

	TaskHandle_t xDDSHandle;
	TaskHandle_t xMonitorHandle;
	TaskHandle_t xDDTGHandle;

	xTaskCreate(xDDS_Task,
			"DDS",
			1024,
		    NULL,
			MAX_USER_TASK_PRIORITY,
			&xDDSHandle);

	xTaskCreate(xMonitor_Task, "MT",
							   1024,
							   NULL, MAX_USER_TASK_PRIORITY - 2,
							   &xMonitorHandle);

	xTaskCreate(xDeadline_Driven_Task_Generator, "DDTG",
							   1024,
							   NULL,
							   MAX_USER_TASK_PRIORITY - 1,
							   &xDDTGHandle);

	//create the monitor task

	// create the generator task
	//xTaskCreate(xMonitor_Task)
	//xTaskCreate()


	vTaskStartScheduler();

	return 0;
}

/*-----------------------------------------------------------*/

void xDDS_Task()
{
	while(1)
	{

		dd_task *req_dd_task;
		if (xQueueReceive(xCreateQueue, &req_dd_task, 0) == pdPASS)
		{
			dd_task *new_dd_task = pvPortMalloc(sizeof(dd_task));
			new_dd_task->t_handle = req_dd_task->t_handle;
			new_dd_task->task_id = req_dd_task->task_id;
			new_dd_task->release_time = xTaskGetTickCount();
			new_dd_task->absolute_deadline = req_dd_task->release_time + req_dd_task->absolute_deadline;
			new_dd_task->completion_time = 0;

			//implement these
			insert_sorted(new_dd_task); // implemented
			assign_priorities(); //implemented
		}
		uint32_t completed_id;
		if (xQueueReceive(xCompleteQueue, &completed_id, 0) == pdPASS)
		{
			//move to completed finds the id and sets the completion time
			move_to_completed(completed_id);
			assign_priorities();
		}

		// check_overdue_tasks();

		uint32_t currentTime = (uint32_t)xTaskGetTickCount();
		check_overdue_tasks(currentTime);

		dd_task_list_req *req;
		if (xQueueReceive(xReqQueue, &req, 0) == pdPASS)
		{
			uint32_t dd_count_msg = 0;

			dd_task_node *curr = NULL;
			if (*req==GET_ACTIVE_LIST)
			{
				curr = active_list;

			}
			else if (*req==GET_COMPLETED_LIST)
			{
				curr = completed_list;
			}
			else if (*req==GET_OVERDUE_LIST)
			{
				curr = overdue_list;
			}
			while (curr!=NULL)
			{
				dd_count_msg++;
				curr = curr->next;
			}
			xQueueSend(xRespQueue, &dd_count_msg, portMAX_DELAY);
		}

		vTaskDelay(1);

	}

}
/*-----------------------------------------------------------*/

void xDeadline_Driven_Task_Generator()
{
	workload_type next = WORKLOAD_1;

	while(1)
	{
		TaskHandle_t h; //I guess it creates a new handle every loop
						//And it doesn't matter what the handles called...

		switch(next)
		{
		case WORKLOAD_1:
		{

			xTaskCreate(xWorkloadTask_1, "WLT1", 1024, NULL, tskIDLE_PRIORITY, &h);
			release_dd_task(h, PERIODIC, 1, pdMS_TO_TICKS(95));
			next = WORKLOAD_2;
			vTaskDelay(500);
			break;
		}
		case WORKLOAD_2:
		{
			xTaskCreate(xWorkloadTask_2, "WLT2", 1024, NULL, tskIDLE_PRIORITY, &h);
			release_dd_task(h, PERIODIC, 2, pdMS_TO_TICKS(150));

			next = WORKLOAD_3;
			vTaskDelay(750);
			break;
		}
		case WORKLOAD_3:
		{
			xTaskCreate(xWorkloadTask_3, "WLT3", 1024, NULL, tskIDLE_PRIORITY, &h);
			release_dd_task(h, PERIODIC, 3, pdMS_TO_TICKS(250));
			next = WORKLOAD_1;
			vTaskDelay(500);
			break;
		}
		}

	}

}



/*-----------------------------------------------------------*/
// User Defined Tasks
// Tasks need to run for their execution time
// I think I have to use for loops
void xWorkloadTask_1(void *pvParameters)
{
	uint32_t id = *(uint32_t *)pvParameters;
	vPortFree(pvParameters);
	uint32_t start = xTaskGetTickCount();
	while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(95));
	complete_dd_task(id); // NO
	vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/

void xWorkloadTask_2(void *pvParameters)
{
	uint32_t id = *(uint32_t *)pvParameters;
	vPortFree(pvParameters);
	uint32_t start = xTaskGetTickCount();
	while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(150));
	complete_dd_task(id);
	vTaskDelete(NULL); // Not sure if this should be inside
					   // complete_dd_task
}
/*-----------------------------------------------------------*/
void xWorkloadTask_3(void *pvParameters)
{
	uint32_t id = *(uint32_t *)pvParameters;
	vPortFree(pvParameters);
	uint32_t start = xTaskGetTickCount();
	while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(250));
	complete_dd_task(id); // NO
	vTaskDelete(NULL);
}
/*-----------------------------------------------------------*/




void release_dd_task(TaskHandle_t t_handle,
					task_type type,
					uint32_t task_id,
					uint32_t absolute_deadline)
{
	dd_task new_task;
	new_task.t_handle = t_handle;
	new_task.type = PERIODIC;
	new_task.task_id = task_id;
	new_task.absolute_deadline = absolute_deadline;
	// DDS assigns release time and completion time
	xQueueSend(xReleaseQueue, &new_task, pdMS_TO_TICKS(100));
}

void insert_sorted(dd_task *new_task)
{
	dd_task_node *node = pvPortMalloc(sizeof(dd_task_node));
	node->task = new_task;
	node->next = NULL;

	if (active_list == NULL ||
			new_task->absolute_deadline < active_list->task->absolute_deadline)
	{
		node->next = active_list;
		active_list = node;
		return;
	}

	dd_task_node *curr = active_list;
	while (curr->next != NULL && curr->next->task->absolute_deadline < new_task->absolute_deadline)
	{
		curr = curr->next;
	}

	node->next = curr->next;
	curr->next = node;
}

void move_to_completed(uint32_t the_task_id)
{
	dd_task_node *prev = NULL;
	dd_task_node *curr = active_list;

	while (curr != NULL)
	{
		if (the_task_id == curr->task->task_id)
		{
			if (prev==NULL)
			{
				active_list = curr->next;
			}
			else
			{
				prev->next = curr->next;
			}
			curr->task->completion_time = xTaskGetTickCount();
			curr->next = completed_list;
			completed_list = curr;

			//update the completion time
			//and move to front of completed list
			//latest completion time first in the
			//linked list
			return;

		}
		prev = curr;
		curr = curr->next;
	}
}

void assign_priorities()
{
	dd_task_node *curr = active_list;
	int prio = MAX_USER_TASK_PRIORITY - 1;

	while (curr != NULL)
	{
		vTaskPrioritySet(curr->task->t_handle, prio);
		prio--;
		curr = curr->next;
	}
}

void check_overdue_tasks(uint32_t currentTime)
{
	dd_task_node *prev = NULL;
	dd_task_node *curr = active_list;

	while (curr!= NULL)
	{
		if (curr->task->absolute_deadline < currentTime)
		{

			dd_task_node *overdue = curr;


			if (prev==NULL)
			{

				active_list = curr->next;

			}
			else
			{
				prev->next = curr->next;
			}
			curr = curr->next;

			overdue->next = overdue_list;
			overdue_list = overdue;

			continue;

		}
		prev = curr;
		curr = curr->next;
	}
}

void complete_dd_task(uint32_t id)
{
	xQueueSend(xCompleteQueue, &id, 0); // wait 0 seconds
}

uint32_t get_completed_task_list(void)
{
	dd_task_list_req req = GET_COMPLETED_LIST;
	xQueueSend(xReqQueue, &req, portMAX_DELAY);

	uint32_t count;
	xQueueReceive(xRespQueue, &count, portMAX_DELAY);

	return count;
}
uint32_t get_overdue_task_list(void)
{
	dd_task_list_req req = GET_OVERDUE_LIST;
	xQueueSend(xReqQueue, &req, portMAX_DELAY);

	uint32_t count;
	xQueueReceive(xRespQueue, &count, portMAX_DELAY);
	return count;
}
uint32_t get_active_task_list(void)
{
	dd_task_list_req req = GET_ACTIVE_LIST;
	xQueueSend(xReqQueue, &req, portMAX_DELAY);

	uint32_t count;
	xQueueReceive(xRespQueue, &count, portMAX_DELAY);

	return count;
}


/*----------------------------------------------------------------*/

void xMonitor_Task(void *pvParameters)
{
	unsigned int num_active;
	unsigned int num_overdue;
	unsigned int num_completed;
	num_active = (unsigned int)get_active_task_list();
	num_completed = (unsigned int)get_overdue_task_list();
	num_overdue = (unsigned int)get_completed_task_list();


	printf("Active: %u, Completed: %u, Overdue: %u\n",
			num_active, num_completed, num_overdue);

	vTaskDelay(pdMS_TO_TICKS(1500)); // One HyperPeriod
}
void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/



