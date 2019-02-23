#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_system.h"

SemaphoreHandle_t one_Hz_sem;
QueueHandle_t data_q_handle;

QueueSetHandle_t data_or_sem_qs_handle;

void producer_1Hz_sem(void *p)
{
	while(true)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		xSemaphoreGive(one_Hz_sem);
	}
}

void producer_data(void *p)
{
	int x = 0;
	
    while(true)
    {
        ++x;
        xQueueSend(data_q_handle, &x, 0);
        int randint = (rand() % 1000) + 100;
        vTaskDelay(randint / portTICK_PERIOD_MS);
    }
}

void processor(void *p)
{
	int samples[10];
	int count = 0;	
	int fatal = 0;
	
    while(true)
    {
    	/* 
    		Do not take semaphore or receive from Queue directly
    		ex: xSemaphoreTake(one_Hz_sem);
    	*/
    	
    	QueueSetMemberHandle_t who_unblocked = xQueueSelectFromSet(data_or_sem_qs_handle, 2000 / portTICK_PERIOD_MS);
    	
    	if(who_unblocked == one_Hz_sem)
    	{
    		if(xSemaphoreTake(one_Hz_sem, 10))
    		{
    			float avg = 0;

    			for(int i = 0; i < count; i++)
    			{
    				avg += samples[i];
    			}
    			avg /= count;
    				
    			count = 0;
    			printf("1Hz: Average of samples = %0.1f\n", avg);
    		}

    		else
    		{
    			puts("ERROR, should never happen");
    			fatal = 1;
    		}
    	}
        
        else if(who_unblocked == data_q_handle)
        {
        	int x = 0;

        	if(xQueueReceive(data_q_handle, &x, 0))
        	{
				if(count<10)
        		{
        			samples[count++] = x;
        			printf("Received %i\n", x);
        		}
        		
        		else
        		{
        			printf("Received value discarded \(Recieved %i\)\n", x);
        		}
        	}

        	else
    		{
    			puts("ERROR, should never happen");
    			fatal = 1;
    		}
        }
        
        else
    	{
    		puts("Invalid case");
    	}
    	
    	if(fatal != 0)
    	{
    		printf("Restarting system now\n");
			vTaskDelay(2000 / portTICK_PERIOD_MS);   
			esp_restart();
    	}
    }
}

void app_main()
{
	/* Initialize Semaphore(Binary) */
	one_Hz_sem = xSemaphoreCreateBinary();
	/* Initialize Queue */
	data_q_handle = xQueueCreate(10, sizeof(int));
	
	/* Initialize QueueSetHandle
	   1. Make sure that the queues or semaphores you are absorbing are empty
	   2. size of QueueSet = size of all queues + size of all semaphores
	      							or
	      size of QueueSet = size of all queues + number of semaphores; since semaphore size is one bit
	*/
	data_or_sem_qs_handle =  xQueueCreateSet(11);
	
	/* Associate the semaphore and queue to the queue set handle */
	xQueueAddToSet(one_Hz_sem, data_or_sem_qs_handle);
	xQueueAddToSet(data_q_handle, data_or_sem_qs_handle);	
	
    /* create new task */
    xTaskCreate(&producer_1Hz_sem, "t1", 1024, NULL, 1, NULL);
    xTaskCreate(&producer_data, "t2", 1024, NULL, 1, NULL);
    xTaskCreate(&processor, "processor", 2048, NULL, 2, NULL);

	while(true);
}
