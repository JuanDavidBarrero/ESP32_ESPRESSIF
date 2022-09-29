#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_slave.h"

#define GPIO_MOSI 23
#define GPIO_MISO 19
#define GPIO_SCLK 18
#define GPIO_CS 5

#define RCV_HOST VSPI_HOST

char recvbuf[129];
char txbuf[129];
int count = 0;

void app_main()
{

    ESP_LOGI("MAIN", "SPI SLAVE START");

    esp_err_t ret;

    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    spi_slave_interface_config_t slvcfg = {
        .mode = 0,
        .spics_io_num = GPIO_CS,
        .queue_size = 3,
        .flags = 0,
        .post_setup_cb = NULL,
        .post_trans_cb = NULL};

    spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    assert(ret == ESP_OK);
    spi_slave_transaction_t data_rx;
    memset(&data_rx, 0, sizeof(data_rx));


    while (true)
    {

        sprintf(txbuf,"All done ! tries -> %i",count);

        data_rx.length = sizeof(recvbuf)*8;
        data_rx.tx_buffer =txbuf; 
        data_rx.rx_buffer=recvbuf;
        ret = spi_slave_transmit(RCV_HOST, &data_rx, portMAX_DELAY);

        if (ret){
            printf(recvbuf);
            printf("\n");
        }

        count++;
    }
    
}