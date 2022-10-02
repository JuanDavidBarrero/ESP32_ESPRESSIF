#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ssd1306.h"
#include "font8x8_basic.h"

#define tag "SSD1306"

char txBuff[32];
int count = 0;

void app_main(void)
{
    SSD1306_t dev;
    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

    ssd1306_init(&dev, 128, 64);
    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xff);
    
    ssd1306_display_text_x3(&dev, 0, "Hello", 5, true);
    ssd1306_display_text(&dev, 4, "Hola mundo", 10, false);
    ssd1306_display_text(&dev, 5, "Hades", 5, false);
    ssd1306_display_text(&dev, 6, "prueba oled", 11, false);
    while (true)
    {
        sprintf(txBuff,"El num es %d",count++);
        ssd1306_display_text(&dev,7,txBuff,strlen(txBuff),false);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    
}