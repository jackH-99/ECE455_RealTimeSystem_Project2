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
#include<stdbool.h>




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
	uint32_t work_id;
	uint32_t release_time;
	uint32_t absolute_deadline;
	uint32_t completion_time;
	SemaphoreHandle_t sem;
	uint32_t instance_id;
} dd_task;

//(TASK LIST) called node instead of list
typedef struct dd_task_node
{
	dd_task task;
	struct dd_task_node *next;
} dd_task_node;

dd_task_node *active_list = NULL;
dd_task_node *completed_list = NULL;
dd_task_node *overdue_list = NULL;


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
					uint32_t absolute_deadline,
					SemaphoreHandle_t sem,
					uint32_t instance_id);

void complete_dd_task(uint32_t instance_id);
void adjust_priorities(void);

void xMonitor_Task(void *pvParameters);

uint32_t get_active_task_list(void);
uint32_t get_overdue_task_list(void);
uint32_t get_completed_task_list(void);

void check_overdue_tasks(uint32_t currentTime);
bool move_to_completed(uint32_t instance_id);
void insert_sorted(dd_task new_task);
void dispatch_head_task_if_idle(void);

void xWorkloadTask_1(void *pvParameters);
void xWorkloadTask_2(void *pvParameters);
void xWorkloadTask_3(void *pvParameters);

SemaphoreHandle_t worker1_sem;
SemaphoreHandle_t worker2_sem;
SemaphoreHandle_t worker3_sem;
QueueHandle_t worker1_instance_q;
QueueHandle_t worker2_instance_q;
QueueHandle_t worker3_instance_q;

void xDeadline_Driven_Task_Generator(void *pvParameters);
void xDDS_Task(void *pvParameters);

/*
QueueHandle_t xRespQueue = NULL;
QueueHandle_t xReqQueue = NULL;
QueueHandle_t xCompleteQueue = NULL;
QueueHandle_t xReleaseQueue = NULL;
*/


QueueHandle_t xCommandQueue = NULL;
QueueHandle_t xOverdueQueue = NULL;
QueueHandle_t xActiveQueue = NULL;
QueueHandle_t xCompleteQueue = NULL;

typedef enum {
	CMD_RELEASE,
	CMD_COMPLETE,
	CMD_MONITOR
} dds_cmd_type;

typedef struct {
	dds_cmd_type type;
	dd_task task;
	uint32_t value;
	dd_task_list_req req;
} command;


TaskHandle_t xWT1Handle;
TaskHandle_t xWT2Handle;
TaskHandle_t xWT3Handle;


TaskHandle_t xDDSHandle;
TaskHandle_t xMonitorHandle;
TaskHandle_t xDDTGHandle;

static bool g_dispatch_in_flight = false;
static uint32_t g_running_instance_id = 0;

#define MAX_USER_TASK_PRIORITY (configMAX_PRIORITIES - 1)





/*-----------------------------------------------------------*/

int main(void)
{



	/*
	xRespQueue = xQueueCreate(5, sizeof(uint32_t));
	xReqQueue = xQueueCreate(5, sizeof(dd_task_list_req));
	xReleaseQueue = xQueueCreate(10, sizeof(dd_task));
	xCompleteQueue = xQueueCreate(10, sizeof(TaskHandle_t));
	*/

	xCommandQueue = xQueueCreate(5, sizeof(command));
	xOverdueQueue = xQueueCreate(5, sizeof(uint32_t));
	xActiveQueue = xQueueCreate(5, sizeof(uint32_t));
	xCompleteQueue = xQueueCreate(5, sizeof(uint32_t));
	worker1_instance_q = xQueueCreate(5, sizeof(uint32_t));
	worker2_instance_q = xQueueCreate(5, sizeof(uint32_t));
	worker3_instance_q = xQueueCreate(5, sizeof(uint32_t));



	xTaskCreate(xDDS_Task,
			"DDS",
			512,
		    NULL,
			MAX_USER_TASK_PRIORITY,
			&xDDSHandle);

	xTaskCreate(xMonitor_Task, "MT",
							   128,
							   NULL, 2,
							   &xMonitorHandle);

	xTaskCreate(xDeadline_Driven_Task_Generator, "DDTG",
							   256,
							   NULL,
							   2,
							   &xDDTGHandle);

	printf("Free heap before WLs: %u\n" , xPortGetFreeHeapSize());
	xTaskCreate(xWorkloadTask_3, "WLT3", 128, NULL, 2, &xWT3Handle);
	xTaskCreate(xWorkloadTask_2, "WLT2", 128, NULL, 2, &xWT2Handle);
	xTaskCreate(xWorkloadTask_1, "WLT1", 128, NULL, 2, &xWT1Handle);

	worker1_sem = xSemaphoreCreateBinary();
	worker2_sem = xSemaphoreCreateBinary();
	worker3_sem = xSemaphoreCreateBinary();




	vTaskStartScheduler();

	return 0;
}

/*-----------------------------------------------------------*/

void dispatch_head_task_if_idle(void)
{
	if (g_dispatch_in_flight || active_list == NULL)
	{
		return;
	}

	uint32_t instance_id = active_list->task.instance_id;
	BaseType_t queued = pdFAIL;

	if (active_list->task.task_id == 1)
	{
		queued = xQueueSend(worker1_instance_q, &instance_id, 0);
		if (queued == pdPASS)
		{
			xSemaphoreGive(worker1_sem);
		}
	}
	else if (active_list->task.task_id == 2)
	{
		queued = xQueueSend(worker2_instance_q, &instance_id, 0);
		if (queued == pdPASS)
		{
			xSemaphoreGive(worker2_sem);
		}
	}
	else if (active_list->task.task_id == 3)
	{
		queued = xQueueSend(worker3_instance_q, &instance_id, 0);
		if (queued == pdPASS)
		{
			xSemaphoreGive(worker3_sem);
		}
	}

	if (queued == pdPASS)
	{
		g_dispatch_in_flight = true;
		g_running_instance_id = instance_id;
	}
}

/*-----------------------------------------------------------*/

void xDDS_Task(void *pvParameters)
{
	(void)pvParameters;
	while(1)
	{

		command msg;
		if (xQueueReceive(xCommandQueue, &msg, portMAX_DELAY))
		{
			switch(msg.type)
			{
				case CMD_RELEASE:
				{
					dd_task new_dd_task;
					new_dd_task.t_handle = msg.task.t_handle;
					new_dd_task.task_id = msg.task.task_id;
					new_dd_task.release_time = xTaskGetTickCount();
					new_dd_task.type = msg.task.type;
					new_dd_task.absolute_deadline = new_dd_task.release_time + msg.task.absolute_deadline;
					new_dd_task.completion_time = 0;
					new_dd_task.sem = msg.task.sem;
					new_dd_task.instance_id = msg.task.instance_id;
					insert_sorted(new_dd_task);

					adjust_priorities();
					break;
				}
					
				case CMD_COMPLETE:
				{
					uint32_t instance_id = msg.value;
					if (instance_id == g_running_instance_id)
					{
						g_dispatch_in_flight = false;
					}

					if (!move_to_completed(instance_id))
					{
						printf("Completion ignored for some reason");
						adjust_priorities();
						break;
					}
					else
					{
						printf("Instance %d completed at %d \n", (int)instance_id, (int)xTaskGetTickCount());
						adjust_priorities();
						break;
					}
				}
				case CMD_MONITOR:
				{
					dd_task_list_req req = msg.req;
					uint32_t dd_count_msg = 0;

					dd_task_node *curr = NULL;
					if (req==GET_ACTIVE_LIST)
					{
						curr = active_list;
						while (curr!=NULL)
						{
						dd_count_msg++;
						curr = curr->next;
						}
						xQueueSend(xActiveQueue, &dd_count_msg, portMAX_DELAY);
						break;

					}
					else if (req==GET_COMPLETED_LIST)
					{
						curr = completed_list;
						while (curr!=NULL)
						{
						dd_count_msg++;
						curr = curr->next;
						}
						xQueueSend(xCompleteQueue, &dd_count_msg, portMAX_DELAY);
						break;
					}
					else if (req==GET_OVERDUE_LIST)
					{
						curr = overdue_list;
						while (curr!=NULL)
						{
						dd_count_msg++;
						curr = curr->next;
						}
						xQueueSend(xOverdueQueue, &dd_count_msg, portMAX_DELAY);
						break;
					}

				}
			}

		}
		check_overdue_tasks((uint32_t)xTaskGetTickCount());
		dispatch_head_task_if_idle();


	}
}


/*-----------------------------------------------------------*/

void xDeadline_Driven_Task_Generator(void *pvParameters)
{
	(void)pvParameters;

	TickType_t next1 = xTaskGetTickCount() + pdMS_TO_TICKS(500);
	TickType_t next2 = xTaskGetTickCount() + pdMS_TO_TICKS(500);
	TickType_t next3 = xTaskGetTickCount() + pdMS_TO_TICKS(750);
	uint32_t firstTime1 = 1, firstTime2 = 1, firstTime3 = 1;
	uint32_t next_instance_id = 1;

	while(1)
	{
		TickType_t now = xTaskGetTickCount();



		if (now >= next1 || firstTime1 == 1)
		{
		next1 += pdMS_TO_TICKS(500);
		firstTime1 = 0;
		release_dd_task(xWT1Handle, PERIODIC, 1, pdMS_TO_TICKS(500), worker1_sem, next_instance_id++);
		}
		if (now >= next2 || firstTime2 == 1)
		{
		firstTime2 = 0;
		next2 += pdMS_TO_TICKS(500);
		release_dd_task(xWT2Handle, PERIODIC, 2, pdMS_TO_TICKS(500), worker2_sem, next_instance_id++);
		}
		if (now >= next3 || firstTime3 == 1)
		{
		next3 += pdMS_TO_TICKS(750);
		firstTime3 = 0;
		release_dd_task(xWT3Handle, PERIODIC, 3, pdMS_TO_TICKS(750), worker3_sem, next_instance_id++);
		}

		vTaskDelay(10);

	}

}



/*-----------------------------------------------------------*/
// User Defined Tasks
// Tasks need to run for their execution time
// I think I have to use for loops
void xWorkloadTask_1(void *pvParameters)
{
	uint32_t start;
	uint32_t instance_id;
	for (;;)
	{
		xSemaphoreTake(worker1_sem, portMAX_DELAY);
		xQueueReceive(worker1_instance_q, &instance_id, portMAX_DELAY);
		start = xTaskGetTickCount();
		while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(95)){}
		complete_dd_task(instance_id);
	}
}

/*-----------------------------------------------------------*/

void xWorkloadTask_2(void *pvParameters)
{
	uint32_t start;
	uint32_t instance_id;
	for (;;)
	{
		xSemaphoreTake(worker2_sem, portMAX_DELAY);
		xQueueReceive(worker2_instance_q, &instance_id, portMAX_DELAY);
		start = xTaskGetTickCount();
		while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(150)){}
		complete_dd_task(instance_id);
	}
}
/*-----------------------------------------------------------*/
void xWorkloadTask_3(void *pvParameters)
{
	uint32_t start = 0;
	uint32_t instance_id;
	for (;;)
	{
		xSemaphoreTake(worker3_sem, portMAX_DELAY);
		xQueueReceive(worker3_instance_q, &instance_id, portMAX_DELAY);
		start = xTaskGetTickCount();
		while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(250)){}
		complete_dd_task(instance_id);
	}
}
/*-----------------------------------------------------------*/




void release_dd_task(TaskHandle_t t_handle,
					task_type type,
					uint32_t task_id,
					uint32_t absolute_deadline, SemaphoreHandle_t sem, uint32_t instance_id)
{
	command msg;
	msg.type = CMD_RELEASE;
	msg.task.t_handle = t_handle;
	msg.task.type = type;
	msg.task.task_id = task_id;
	msg.task.absolute_deadline = absolute_deadline;
	msg.task.sem = sem;
	msg.task.instance_id = instance_id;
	msg.task.release_time = 0;
	msg.task.completion_time = 0;

	xQueueSend(xCommandQueue, &msg, 0);
}

void insert_sorted(dd_task new_task)
{
	// This function inserts a new task into the active list, sorted by absolute deadline
	dd_task_node *node = pvPortMalloc(sizeof(dd_task_node));
	node->task = new_task;
	node->next = NULL;

	// If list is empty, insert as head
	if (active_list == NULL)
	{
		active_list = node;
		return;
	}

	if (new_task.absolute_deadline < active_list->task.absolute_deadline)
	{
		node->next = active_list;
		active_list = node;
		return;
	}

	// Search for insertion point.
	dd_task_node *curr = active_list;
	while (curr->next != NULL && curr->next->task.absolute_deadline < new_task.absolute_deadline)
	{
		curr = curr->next;
	}

	node->next = curr->next;
	curr->next = node;
}

void adjust_priorities()
{
	if (active_list == NULL) return;
	dd_task_node *curr = active_list;
	vTaskPrioritySet(curr->task.t_handle, MAX_USER_TASK_PRIORITY);
	if (curr->next != NULL)
	{
		curr = curr->next;
		vTaskPrioritySet(curr->task.t_handle, MAX_USER_TASK_PRIORITY - 1);
	}
		
	if (curr->next != NULL)
	{
		curr = curr->next;
		vTaskPrioritySet(curr->task.t_handle, MAX_USER_TASK_PRIORITY - 2);
	}
	
}


bool move_to_completed(uint32_t instance_id)
{
	dd_task_node *prev = NULL;
	dd_task_node *curr = active_list;

	while (curr != NULL)
	{
		if (instance_id == curr->task.instance_id)
		{

			if (prev==NULL)
			{
				active_list = curr->next;
			}
			else
			{
				prev->next = curr->next;
			}
			curr->task.completion_time = xTaskGetTickCount();
			curr->next = completed_list;
			completed_list = curr;
			return true;

		}
		prev = curr;
		curr = curr->next;
	}
	return false;
}



void check_overdue_tasks(uint32_t currentTime)
{
	dd_task_node *prev = NULL;
	dd_task_node *curr = active_list;

	while (curr!= NULL)
	{
		if (curr->task.absolute_deadline < currentTime)
		{

			dd_task_node *overdue = curr;
			bool was_running_instance = (overdue->task.instance_id == g_running_instance_id);


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

			if (was_running_instance)
			{
				g_dispatch_in_flight = false;
				g_running_instance_id = 0;
			}

			continue;

		}
		prev = curr;
		curr = curr->next;
	}
}

void complete_dd_task(uint32_t instance_id)
{
	command msg;
	msg.type = CMD_COMPLETE;
	msg.value = instance_id;
	xQueueSend(xCommandQueue, &msg, 0); // wait 0 seconds
}

uint32_t get_completed_task_list(void)
{
	uint32_t count = 0;
	dd_task_list_req req = GET_COMPLETED_LIST;
	command msg;
	msg.type = CMD_MONITOR;
	msg.req = req;
	xQueueSend(xCommandQueue, &msg, portMAX_DELAY);
	xQueueReceive(xCompleteQueue, &count, portMAX_DELAY);
	return count;
}

uint32_t get_overdue_task_list(void)
{
	uint32_t count = 0;
	dd_task_list_req req = GET_OVERDUE_LIST;
	command msg;
	msg.type = CMD_MONITOR;
	msg.req = req;
	xQueueSend(xCommandQueue, &msg, portMAX_DELAY);
	xQueueReceive(xOverdueQueue, &count, portMAX_DELAY);
	return count;
}
uint32_t get_active_task_list(void)
{
	uint32_t count = 0;
	dd_task_list_req req = GET_ACTIVE_LIST;
	command msg;
	msg.type = CMD_MONITOR;
	msg.req = req;
	xQueueSend(xCommandQueue, &msg, portMAX_DELAY);
	xQueueReceive(xActiveQueue, &count, portMAX_DELAY);
	return count;
}


/*----------------------------------------------------------------*/

void xMonitor_Task(void *pvParameters)
{
	for(;;){
	vTaskDelay(pdMS_TO_TICKS(1500)); // One HyperPeriod
	(void)pvParameters;
	unsigned int num_active;
	unsigned int num_overdue;
	unsigned int num_completed;
	num_active = (unsigned int)get_active_task_list();
	num_completed = (unsigned int)get_completed_task_list();
	num_overdue = 	(unsigned int)get_overdue_task_list();


	printf("Active: %u, Completed: %u, Overdue: %u\n",
			num_active, num_completed, num_overdue);
	}

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



