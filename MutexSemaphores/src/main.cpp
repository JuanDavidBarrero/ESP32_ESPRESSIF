#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

xSemaphoreHandle xMutex;

void write_message_console(std::string message)
{
  printf(message.c_str());
}

void temperature_task(void *params)
{
  while (true)
  {
    if (xSemaphoreTake(xMutex, portMAX_DELAY))
    {
      ESP_LOGI("TEMP", "Inside temperature_task");
      write_message_console("temperature is 35°c\n");
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void humidity_task(void *params)
{
  while (true)
  {
    if (xSemaphoreTake(xMutex, portMAX_DELAY))
    {
      ESP_LOGW("HUM", "Inside humidity_task");
      write_message_console("humidity is 48% \n");
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

extern "C" void app_main(void)
{
  xMutex = xSemaphoreCreateMutex();
  xTaskCreate(&temperature_task, "temperature_task", 2048, NULL, 2, NULL);
  xTaskCreate(&humidity_task, "humidity_task", 2048, NULL, 2, NULL);
}