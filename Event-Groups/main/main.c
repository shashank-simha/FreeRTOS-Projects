#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "freertos/event_groups.h"

#define good_task_id 00000000000000000000000000000001
#define bad_task_id 00000000000000000000000000000010

EventGroupHandle_t task_watchdog;

const uint32_t tasks_all_bits = (good_task_id | bad_task_id);

static int stopped = 0;

void good_task(void *p)
{
	while(true)
	{
		//always set the event bits
		xEventGroupSetBits(task_watchdog, good_task_id);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void bad_task(void *p)
{
	int x = 0;
	
	while(true)
	{
		++x;
		
		if(x >=3 && x <= 6)
		{
			// stop setting event bit at the task_watchdog event group 
		}
		
		else
		{
			// set the event bits
			xEventGroupSetBits(task_watchdog, bad_task_id);
		}
		
		vTaskDelay(1000 / portTICK_PERIOD_MS);   
	}
}

void sw_watchdog(void *p)
{
	while(true)
	{
		uint32_t result = xEventGroupWaitBits(task_watchdog, tasks_all_bits,
											  pdTRUE, // clear all bits on exit
											  pdTRUE, // wait for all bits to be set
											  2000 / portTICK_PERIOD_MS
											  );
		
		if((result & tasks_all_bits) == tasks_all_bits)
		{
			printf("System is healthy...\n");
		}
		
		else
		{
			if(!(result & good_task_id))
			{
				printf("Good task stopped responding...\n");
				stopped++;
			}
			
			if(!(result & bad_task_id))
			{
				printf("Bad task stopped responding...\n");
				stopped++;
			}	
		}
		
		if(stopped >= 2)		
		{
			printf("Restarting system now\n");
			vTaskDelay(2000 / portTICK_PERIOD_MS);   
			esp_restart();
		}
	}
}

void app_main()
{
	/* Initialize EventGroup */
	task_watchdog = xEventGroupCreate();

    /* create new task */
    xTaskCreate(&good_task, "task1", 1024, NULL, 1, NULL);
    xTaskCreate(&bad_task, "task2", 1024, NULL, 1, NULL);
    xTaskCreate(&sw_watchdog, "sw_watchdog", 1024, NULL, 2, NULL);	

}
