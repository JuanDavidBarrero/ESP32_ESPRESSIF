#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_master.h"

#define GPIO_MOSI 23
#define GPIO_MISO 19
#define GPIO_SCLK 18
#define GPIO_CS 5

#define SENDER_HOST HSPI_HOST

int count = 0;

void app_main()
{

    ESP_LOGI("MAIN", "SPI MASTER CONFIG");

    esp_err_t ret;
    spi_device_handle_t handle;

    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = GPIO_MOSI;
    buscfg.miso_io_num = GPIO_MISO;
    buscfg.sclk_io_num = GPIO_SCLK;

    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = 5000000,
        .duty_cycle_pos = 128, // 50% duty cycle
        .mode = 0,
        .spics_io_num = GPIO_CS,
        .cs_ena_posttrans = 3, // Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
        .queue_size = 3};

    char sendbuf[128] = {0};
    char recvbuf[128] = {0};
    spi_transaction_t data_tx;
    memset(&data_tx, 0, sizeof(data_tx));

    ret = spi_bus_initialize(SENDER_HOST, &buscfg, SPI_DMA_CH_AUTO);
    assert(ret == ESP_OK);
    ret = spi_bus_add_device(SENDER_HOST, &devcfg, &handle);
    assert(ret == ESP_OK);



    while (true)
    {
        sprintf(sendbuf,"SPI MASTER SEND %i\n",count);
        data_tx.length = sizeof(sendbuf)*8;
        data_tx.tx_buffer = sendbuf;
        data_tx.rx_buffer = recvbuf;
        spi_device_transmit(handle,&data_tx);

        count++;

        printf(recvbuf);

        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    

  
}
