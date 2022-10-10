#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora.h"

typedef struct
{
    char name[5];
    int id;
    int data;
} person_t;


void task_rx(void *p)
{
   person_t LoRaReceiverData;

   for(;;) {
      lora_receive();    // put into receive mode
      while(lora_received()) {
         lora_receive_packet((uint8_t *)&LoRaReceiverData, sizeof(LoRaReceiverData));
         printf("Name: %s id: %i Data: %i \n", LoRaReceiverData.name, LoRaReceiverData.id, LoRaReceiverData.data);
         lora_receive();
      }
      vTaskDelay(1);
   }
}

void app_main()
{
   lora_init();
   lora_set_frequency(433e6);
   lora_enable_crc();
   xTaskCreate(&task_rx, "task_rx", 2048, NULL, 5, NULL);
}