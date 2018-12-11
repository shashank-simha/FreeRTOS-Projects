#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
 
xQueueHandle Message_Queue_Handle;

void sender_task(void *pvParameter)
{
 int tx_int = 0;
 while(true)
 {
  printf("Sending %d to the receiver task\n", tx_int);
  if(! xQueueSend(Message_Queue_Handle, &tx_int, 2000))
  {
   printf("Failed to send to the queue\n");
  }
  tx_int++;
  vTaskDelay(1000 / portTICK_PERIOD_MS);
 }
}

void receiver_task(void *pvParameter)
{
 int rx_int = 0;
 while(true)
 {
  if(xQueueReceive(Message_Queue_Handle, &rx_int, 1000))
  {
   printf("Received %d from the sender task\n", rx_int);
  }
  else
  {
   printf("Failed to receive data from queue\n");
  }
 }
}

void app_main()
{
	/* Initialize Queue */
	Message_Queue_Handle = xQueueCreate(3, sizeof(int));	
    /* create new task */
    xTaskCreate(&sender_task, "tx_task", 2048, NULL, 1, NULL);
    xTaskCreate(&receiver_task, "rx_task", 2048, NULL, 1, NULL);
}
