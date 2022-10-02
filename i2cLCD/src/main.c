#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "HD44780.h"

#define LCD_ADDR 0x27
#define SDA_PIN 21
#define SCL_PIN 22
#define LCD_COLS 20
#define LCD_ROWS 4

static char tag[] = "LCD test";
void LCD_DemoTask(void *param);

void app_main(void)
{
    ESP_LOGI(tag, "Starting up application");
    LCD_init(LCD_ADDR, SDA_PIN, SCL_PIN, LCD_COLS, LCD_ROWS);
    xTaskCreate(&LCD_DemoTask, "Demo Task", 4096, NULL, 10, NULL);
}

void LCD_DemoTask(void *param)
{
    char num[20];
    int count = 0;
    for (;;)
    {
        LCD_setCursor(0, 0);
        LCD_writeStr("Hola mundo");
        LCD_setCursor(0, 1);
        LCD_writeStr("Hola mundo hades");
        LCD_setCursor(0, 2);
        LCD_writeStr("Prueba de lcd");
        LCD_setCursor(0, 3);
        sprintf(num, "num is %d", count++);
        LCD_writeStr(num);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}