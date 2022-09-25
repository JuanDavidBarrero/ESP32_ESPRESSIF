#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "driver/ledc.h"

#define TAG "MAIN"

int potValue =0;

extern "C" void app_main()
{

    ESP_LOGI(TAG,"Inicio del proyecto");

    adc2_config_channel_atten(ADC2_CHANNEL_0,ADC_ATTEN_DB_11);
    // adc1_config_width(ADC_WIDTH_BIT_12);
    // adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);

    ledc_timer_config_t ledc_timer = {};
    ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;
    ledc_timer.freq_hz = 5000;
    ledc_timer.timer_num = LEDC_TIMER_0;
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {};
    ledc_channel.channel = LEDC_CHANNEL_0;
    ledc_channel.duty =0;
    ledc_channel.gpio_num = GPIO_NUM_2;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&ledc_channel);

    while (true)
    {
        adc2_get_raw(ADC2_CHANNEL_0,ADC_WIDTH_BIT_10,&potValue);
        printf("El valor del adc es: %i\n",potValue);
        ledc_set_duty(ledc_channel.speed_mode,ledc_channel.channel,potValue);
        ledc_update_duty(ledc_channel.speed_mode,ledc_channel.channel);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    
}