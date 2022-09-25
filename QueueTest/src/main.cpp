#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include <string>
#include "test.h"

#define TAG "MAIN"

QueueHandle_t mgs_queue;
int mgs_queue_len = 5;

void Task1(void *arg);
void Task2(void *arg);

typedef struct
{
    std::string nombre;
    int edad;
} Persona_t;

Persona_t Juan;

extern "C" void app_main()
{

    mgs_queue = xQueueCreate(mgs_queue_len, sizeof(Persona_t));

    xTaskCreatePinnedToCore(&Task1, "Task1", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(&Task2, "Task2", 2048, &Juan, 1, NULL, 1);
}

void Task1(void *arg)
{

    Persona_t recv;

    ESP_LOGW(TAG, "Iniciando tarea 1");

    for (;;)
    {
        if (xQueueReceive(mgs_queue, &recv, 0) == pdTRUE)
        {
            printf("the name is %s and is %i years old \n", recv.nombre.c_str(), recv.edad);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
void Task2(void *arg)
{

    Persona_t person  = *(Persona_t *)arg;

    ESP_LOGW(TAG, "Iniciando tarea 2");

    person.nombre = "JuanDavid";

    for (;;)
    {
        if (xQueueSend(mgs_queue, &person, 10) != pdTRUE)
        {
            ESP_LOGE(TAG, "QUEUE is full");
        }

        person.edad++;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}