#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora.h"

int count = 0;

typedef struct
{
    char name[5];
    int id;
    int data;
} person_t;

void task_tx(void *p)
{

    person_t LoRaSenderData;
    LoRaSenderData.id = 1234;
    strcpy(LoRaSenderData.name, "Hades");

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        count++;
        LoRaSenderData.data = count;
        lora_send_packet((uint8_t *)&LoRaSenderData, sizeof(LoRaSenderData));
        printf("packet sent...\n");
    }
}

void app_main()
{
    lora_init();
    lora_set_frequency(433e6);
    lora_enable_crc();
    xTaskCreate(&task_tx, "task_tx", 2048, NULL, 5, NULL);
}