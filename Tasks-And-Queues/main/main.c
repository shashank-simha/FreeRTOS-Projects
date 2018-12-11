#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"

#define MaxQueueSize 3
#define MaxElementsPerQueue 20

char TxBuffer[5][MaxElementsPerQueue]={"Hello world", "Good Morning", "welcome", "Free Rtos", "Good Bye"};
char RxBuffer[5][MaxElementsPerQueue];

xTaskHandle SendTaskHandle;
xTaskHandle ReceiveTaskHandle;
xQueueHandle MessageQueueHandle;

bool DataSent = false;
bool DataReceived = false;

void sender_task(void *pvParameter)
{
 unsigned char tx_int;
 for(tx_int=0;tx_int<5;tx_int++)
 {
  if(! xQueueSend(MessageQueueHandle, TxBuffer[tx_int], 100))
  {
   printf("Failed to send the messages\n");
  }
  else
  {
   printf("Successfully sent data\n");
  }
  vTaskDelay(1000 / portTICK_PERIOD_MS);
 }
 DataSent = true;
 vTaskDelete(SendTaskHandle);
}

void receiver_task(void *pvParameter)
{
 unsigned char rx_int;
 for(rx_int=0;rx_int<5;rx_int++)
 {
  if(xQueueReceive(MessageQueueHandle, RxBuffer[rx_int], 100))
  {
   printf("Received data is: %s\n", RxBuffer[rx_int]);
  }
  else
  {
   printf("No Data received\n");
  }
  vTaskDelay(1000 / portTICK_PERIOD_MS);
 }
 DataReceived = true;
 vTaskDelete(ReceiveTaskHandle);
}

void app_main()
{
	/* Initialize Queue */
	MessageQueueHandle = xQueueCreate(MaxQueueSize, MaxElementsPerQueue);	
	
    /* create new task */
    xTaskCreate(&sender_task, "tx_task", 2048, NULL, 1, &SendTaskHandle);
    xTaskCreate(&receiver_task, "rx_task", 2048, NULL, 1, &ReceiveTaskHandle);
	
	while(1)
	{
	 if(DataSent && DataReceived)
	 {
	  printf("Restarting now.\n");
	  vTaskDelay(1000 / portTICK_PERIOD_MS);
	  fflush(stdout);
	  esp_restart();
	 }
	}
}
