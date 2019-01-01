#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_timer.h"

xSemaphoreHandle BianrySem;

void one_sec_isr()
{
	xSemaphoreGiveFromISR(BianrySem, NULL);
}

void sem_task()
{
	while(true)
	{
		if(xSemaphoreTake(BianrySem, 9999999))
		{
			puts("Tick!");
		}
	}
}

void app_main()
{
	/* Initialize Semaphore(Binary) */
	vSemaphoreCreateBinary(BianrySem);

	/* Initialize ISR */
	const esp_timer_create_args_t periodic_isr_args = {
            .callback = &one_sec_isr,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic_isr"
    };
	
	esp_timer_handle_t periodic_isr;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_isr_args, &periodic_isr));
	
	/* Start the timers */
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_isr, 10000000 / portTICK_PERIOD_MS));    
    
    /* create new task */
    xTaskCreate(&sem_task, "t", 2048, NULL, 1, NULL);
    
	while(true);
}
