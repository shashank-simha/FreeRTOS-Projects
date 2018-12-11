#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
 
void enable_flush_after_print()
{
 setvbuf(stdin, NULL, _IONBF, 0);
 setvbuf(stdout, NULL, _IONBF, 0);
}

void hello_task(void *pvParameter)
{
 int i = 0;
 while(true)
 {
  printf("Hello World %d\n",i);
  i++;
  vTaskDelay(1000 / portTICK_PERIOD_MS);
 }
}

void app_main()
{
    /* create new task */
    xTaskCreate(&hello_task, "print_task", 2048, NULL, 1, NULL);
    
}
