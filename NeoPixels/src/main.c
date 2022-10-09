#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include <led_strip.h>

#define LED_TYPE LED_STRIP_WS2812
#define LED_GPIO CONFIG_LED_STRIP_GPIO
#define LED_STRIP_BRIGHTNESS CONFIG_LED_STRIP_BRIGHTNESS

static const rgb_t colors[] = {
    {.r = 255, .g = 0, .b = 0},
    {.r = 0, .g = 255, .b = 0},
    {.r = 0, .g = 0, .b = 255},
    {.r = 255, .g = 255, .b = 0},
    {.r = 97, .g = 50, .b = 107},
};

const rgb_t blackColor = {
    .red = 0,
    .blue = 0,
    .green = 0,
};

#define COLORS_TOTAL (sizeof(colors) / sizeof(rgb_t))

void test(void *pvParameters)
{
    led_strip_t strip = {
        .type = LED_TYPE,
        .length = CONFIG_LED_STRIP_LEN,
        .gpio = LED_GPIO,
        .buf = NULL,
        .brightness = LED_STRIP_BRIGHTNESS,
    };

    ESP_ERROR_CHECK(led_strip_init(&strip));

    while (1)
    {
        for (int i = 0; i < COLORS_TOTAL; i++)
        {
            ESP_LOGI("Strip", "Mostrando todos los coleres");
            ESP_ERROR_CHECK(led_strip_fill(&strip, 0, 10, colors[i]));
            ESP_ERROR_CHECK(led_strip_flush(&strip));
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        led_strip_fill(&strip, 0, strip.length, blackColor);
        led_strip_flush(&strip);

        for (int i = 0; i < CONFIG_LED_STRIP_LEN; i++)
        {
            ESP_LOGW("Strip", "Mostrando color por color");
            led_strip_fill(&strip, i, 1, colors[0]);
            led_strip_flush(&strip);
            vTaskDelay(pdMS_TO_TICKS(200));
            led_strip_fill(&strip, 0, strip.length, blackColor);
            led_strip_flush(&strip);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}

void app_main()
{
    led_strip_install();
    xTaskCreate(test, "test", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
}
