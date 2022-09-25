#include <stdio.h>
#include <string>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "MAIN"

#define EXAMPLE_NAME CONFIG_USER_NAME
#define EXAMPLE_AGE CONFIG_USER_AGE

extern "C"
{
    void app_main()
    {

        
        int count = 0;
        printf("\n Bienvenido %s listo para empezar\n", EXAMPLE_NAME);

        ESP_LOGI(TAG, "Hola mundo esto es info\n");
        ESP_LOGE(TAG, "Hola mundo error\n");
        ESP_LOGW(TAG, "hola mundo advertencia\n");

        if (CONFIG_USER_AGE < 18)
        {
            ESP_LOGE(TAG, "No puede aceder al codigo");
        }
        else
        {
            while (true)
            {
                ESP_LOGI(TAG, "Hola desde el loop");
                printf("contador: %i\n", count);
                count++;
                vTaskDelay(1000 / portTICK_RATE_MS);
            }
        }
    }
}