#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "MAIN"

#ifdef CONFIG_IS_ACTIVATE
bool state = true;
#else
bool state = false;
#endif

static TickType_t next = 0;
static bool led_state = false;

void toogleButton(void *pvParam);
void buttonHandler(void *arg);

extern "C" void app_main()
{
    ESP_LOGI(TAG, "Inicio del codigo");

    gpio_config_t led = {};
    led.mode = GPIO_MODE_OUTPUT;
    led.pin_bit_mask = (1ULL << GPIO_NUM_2);
    gpio_config(&led);

    if (state)
    {
        ESP_LOGI(TAG, "Iniciando boton por interrupcion");
        gpio_config_t button = {};
        button.mode = GPIO_MODE_INPUT;
        button.pin_bit_mask = (1ULL << GPIO_NUM_5);
        button.pull_up_en = GPIO_PULLUP_ENABLE;
        button.intr_type = GPIO_INTR_NEGEDGE;
        gpio_config(&button);

        gpio_install_isr_service(0);
        gpio_isr_handler_add(GPIO_NUM_5, buttonHandler, NULL);
    }
    else
    {
        ESP_LOGI(TAG, "Iniciando boton");
        xTaskCreatePinnedToCore(&toogleButton, "LedTask", 1024, NULL, 0, NULL, 0);
    }
}

void toogleButton(void *pvParam)
{

    gpio_config_t button = {};
    button.pin_bit_mask = (1ULL << GPIO_NUM_5);
    button.pull_up_en = GPIO_PULLUP_ENABLE;
    button.mode = GPIO_MODE_INPUT;
    gpio_config(&button);

    for (;;)
    {
        if (gpio_get_level(GPIO_NUM_5))
        {
            gpio_set_level(GPIO_NUM_2, 0);
        }
        else
        {
            gpio_set_level(GPIO_NUM_2, 1);
        }
    }
}

void IRAM_ATTR buttonHandler(void *arg)
{
    TickType_t now = xTaskGetTickCountFromISR();
    if (now > next)
    {
        led_state = !led_state;
        gpio_set_level(GPIO_NUM_2, led_state);
        next = now + 500 / portTICK_PERIOD_MS;
    }
}
