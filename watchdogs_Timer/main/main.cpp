#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "esp_log.h"

#define BLINK_GPIO GPIO_NUM_2

extern "C" void app_main(void)
{
    esp_int_wdt_init();

    esp_task_wdt_init(2, true);

    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    printf("Iniciando el codigo\n");
    esp_task_wdt_add(NULL);

    uint8_t count = 0;

    while (true)
    {
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        if (count < 10)
        {
            ESP_LOGW("WDT","Feed the dog");
            esp_task_wdt_reset();
        }
        count++;
        
    }

    
}