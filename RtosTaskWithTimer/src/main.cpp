#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_log.h"

#define TAG "MAIN"

void Task1(void *arg);
void Task2(void *arg);
void Task3(void *arg);
void TaskTimer(void *arg);

int count = 0;
int count2 = 0;

TaskHandle_t taskOneHandle = NULL;
TimerHandle_t xTimer;

extern "C" void app_main()
{
    ESP_LOGW(TAG, "Inicio del programa");
    xTaskCreatePinnedToCore(&Task1, "Task1", 4096, &count, 1, &taskOneHandle, 0);
    xTaskCreatePinnedToCore(&Task2, "Task2", 4096, &count2, 1, NULL, 1);
    xTimer = xTimerCreate("TaskTimer",5000/portTICK_PERIOD_MS,pdTRUE,NULL,&TaskTimer);
}


void TaskTimer(void *arg){
    ESP_LOGW(TAG,"Función llamada por timer");
}

void Task1(void *arg)
{
    int *c = (int *)arg;
    ESP_LOGW(TAG, "Iniciando tarea uno");

    while (true)
    {
        printf("El contador va en %i\n", *c);
        (*c)++;
        if (*c == 5)
        {
            ESP_LOGI(TAG,"Función por timer va iniciar");
            xTimerStart(xTimer,0);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void Task2(void *arg)

{

    int c = *((int *)arg);
    ESP_LOGE(TAG, "Iniciando tarea dos");

    while (true)

    {
        printf("El contador DOS va en %i\n", c);
        (c)++;
        vTaskDelay(500 / portTICK_PERIOD_MS);
        if (c == 10)
        {
            ESP_LOGI(TAG, "Se pauso la tarea uno");
            vTaskSuspend(taskOneHandle);
        }
        if (c == 20)
        {
            ESP_LOGI(TAG, "Se retoma la tarea uno");
            vTaskResume(taskOneHandle);
        }
        if (c == 30)
        {
            ESP_LOGE(TAG, "se elimino la tarea uno");
            vTaskDelete(taskOneHandle);
            ESP_LOGE(TAG, "se elimino El timer");
            xTimerStop(xTimer,0);
            xTaskCreatePinnedToCore(&Task3, "Task3", 4096, NULL, 0, NULL, 0);
        }
    }
}

void Task3(void *arg)
{
    int hola = 100;
    ESP_LOGW(TAG, "Iniciando tarea tres");

    while (true)
    {
        printf("Task tres -> %i\n", hola);
        hola--;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}