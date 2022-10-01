#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"

#define UART_PORT UART_NUM_2
#define TXD_PIN 17
#define RXD_PIN 16
#define UART_BUFF_SIZE 1024

char Sendbuffer[UART_BUFF_SIZE];
char Recvbuffer[UART_BUFF_SIZE];

int count = 0;

void Uarttask(void *args);

void app_main()
{

    ESP_LOGI("MAIN", "Inicio de la prueba UART");

    xTaskCreatePinnedToCore(&Uarttask, "Task", 1024 * 3, NULL, 0, NULL, 0);
}

void Uarttask(void *args)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_PORT, UART_BUFF_SIZE, 0, 0, NULL, 0);
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    for (;;)
    {
        int len = uart_read_bytes(UART_PORT, Recvbuffer, sizeof(Recvbuffer), 20 / portTICK_PERIOD_MS);
        sprintf(Sendbuffer, "Envio de dato %i", count++);
        uart_write_bytes(UART_PORT, Sendbuffer, strlen(Sendbuffer));
        if (len)
        {
            Recvbuffer[len] = '\0';
            ESP_LOGI("UART RX", "El mensaje es %s", Recvbuffer);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
};