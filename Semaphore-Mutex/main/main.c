#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"

xSemaphoreHandle GateKeeper;

void access_resource()
{

}

void user_1(void *pvParameter)
{
 while(true)
 {
  if(xSemaphoreTake(GateKeeper, 1000))
  {
   printf("User 1 got access to the resource\n");
   access_resource();
   vTaskDelay(3000 / portTICK_PERIOD_MS);
   xSemaphoreGive(GateKeeper);
  }
  else
  {
   printf("User 1 failed to get access withing 1 sec\n");
  }
  vTaskDelay(1000 / portTICK_PERIOD_MS);
 }
}

void user_2(void *pvParameter)
{
 while(true)
 {
  if(xSemaphoreTake(GateKeeper, 1000))
  {
   printf("User 2 got access to the resource\n");
   access_resource();
   vTaskDelay(1000 / portTICK_PERIOD_MS);
   xSemaphoreGive(GateKeeper);
  }
  else
  {
   printf("User 2 failed to get access withing 1 sec\n");
  }
  vTaskDelay(1000 / portTICK_PERIOD_MS);
 }
}

void app_main()
{
	/* Initialize Semaphore(Mutex) */
	GateKeeper = xSemaphoreCreateMutex();
	
    /* create new task */
    xTaskCreate(&user_1, "t1", 2048, NULL, 1, NULL);
    xTaskCreate(&user_2, "t2", 2048, NULL, 1, NULL);
	
	while(1);
}
