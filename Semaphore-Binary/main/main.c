#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"

xSemaphoreHandle EmployeeSignal;


void employee_task()
{

}

void boss(void *p)
{
    while(true)
    {
        printf("Boss giving the signal\n");
        xSemaphoreGive(EmployeeSignal);
        printf("Boss finished giving signal\n");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void employee(void *p)
{
    while(true)
    {
        if(xSemaphoreTake(EmployeeSignal, portMAX_DELAY))
        {
            employee_task();
            printf("Employee has completed its task\n");
        }
    }
}

void app_main()
{
	/* Initialize Semaphore(Binary) */
	vSemaphoreCreateBinary(EmployeeSignal);

    /* create new task */
    xTaskCreate(&boss, "t1", 2048, NULL, 1, NULL);
    xTaskCreate(&employee, "t2", 2048, NULL, 1, NULL);

	while(1);
}
